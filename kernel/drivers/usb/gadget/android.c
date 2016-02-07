/*
 * Gadget Driver for Android
 *
 * Copyright (C) 2008 Google, Inc.
 * Author: Mike Lockwood <lockwood@android.com>
 *         Benoit Goby <benoit@android.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/utsname.h>
#include <linux/platform_device.h>

#include <linux/usb/ch9.h>
#include <linux/usb/composite.h>
#include <linux/usb/gadget.h>

#include "gadget_chips.h"

#include "f_fs.c"
#include "f_audio_source.c"
#include "f_mass_storage.c"
#include "f_adb.c"
#include "f_mtp.c"
#include "f_accessory.c"
#define USB_ETH_RNDIS y
#include "f_rndis.c"
#include "rndis.c"
#include "u_ether.c"
#include <linux/rtc.h>

#if defined (CONFIG_HUAWEI_DSM)
#include <huawei_platform/dsm/dsm_pub.h>
#endif

#include "f_hdb.c"

#ifdef FINAL_RELEASE_MODE
#undef HW_USB_TIME_SYNC_PC
#else
#define HW_USB_TIME_SYNC_PC
#endif

MODULE_AUTHOR("Mike Lockwood");
MODULE_DESCRIPTION("Android Composite USB Driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

#define D(format, arg...) pr_info("====>>[%s]" format, __func__, ##arg)

static const char longname[] = "Gadget Android";
static int setup_string_id;

/* Default vendor and product IDs, overridden by userspace */
#define VENDOR_ID       0x12D1
#define PRODUCT_ID      0x1057
#define USB_DEVIEC_PLUGIN  1
#define USB_DEVIEC_PLUGOUT 2
#define USB_REQ_SEND_HOST_TIME    0xA2
#define USB_DEVICE_BCD_CODE 0x9999

struct android_usb_function {
    char *name;
    void *config;

    struct device *dev;
    char *dev_name;
    struct device_attribute **attributes;

    /* for android_dev.enabled_functions */
    struct list_head enabled_list;

    /* Optional: initialization during gadget bind */
    int (*init)(struct android_usb_function *, struct usb_composite_dev *);
    /* Optional: cleanup during gadget unbind */
    void (*cleanup)(struct android_usb_function *);
    /* Optional: called when the function is added the list of
     *        enabled functions */
    void (*enable)(struct android_usb_function *);
    /* Optional: called when it is removed */
    void (*disable)(struct android_usb_function *);

    int (*bind_config)(struct android_usb_function *,
               struct usb_configuration *);

    /* Optional: called when the configuration is removed */
    void (*unbind_config)(struct android_usb_function *,
                  struct usb_configuration *);
    /* Optional: handle ctrl requests before the device is configured */
    int (*ctrlrequest)(struct android_usb_function *,
                    struct usb_composite_dev *,
                    const struct usb_ctrlrequest *);
};

struct android_dev {
    struct android_usb_function **functions;
    struct list_head enabled_functions;
    struct usb_composite_dev *cdev;
    struct device *dev;

    bool enabled;
    int disable_depth;
    struct mutex mutex;
    bool connected;
    bool sw_connected;
    bool disabled;
    struct work_struct work;
    char ffs_aliases[256];
    struct work_struct    notify_work;
    int event;
};

struct _time {
    int            time_bias;
    unsigned short year;
    unsigned short month;
    unsigned short day;
    unsigned short hour;
    unsigned short minute;
    unsigned short second;
    unsigned short millisecond;
    unsigned short weekday;
};

#if defined (CONFIG_HUAWEI_DSM)
static struct dsm_dev dsm_usb = {
	.name = "dsm_usb",
	.device_name = NULL,
	.ic_name = NULL,
	.module_name = NULL,
	.fops = NULL,
	.buff_size = 1024,
};
static struct dsm_client *usb_dclient = NULL;

#define USB_DSM(error_found,dsm_info,func) do{ \
        if (error_found >= 0){\
            dsm_client_record(usb_dclient, dsm_info,\
				func);\
            error_found++;\
        }\
}while(0)
#endif

static struct class *android_class;
static struct android_dev *_android_dev;
static int android_bind_config(struct usb_configuration *c);
static void android_unbind_config(struct usb_configuration *c);
//static int android_function_supported_check(unsigned char * f_name); /* added by hisi-balong */
static int android_enable_function_check(unsigned char *f_name);
static int android_acm_debug_check(void);
static int android_setup_config(struct usb_configuration *c,
        const struct usb_ctrlrequest *ctrl);
/*extern int usb_shell_init(void);*/
extern void BSP_USB_UdiDisableNotify (void);
extern void BSP_USB_UdiEnableNotify (void);
extern int gser_num;
extern int rndis_num;
extern int port_num;

/* string IDs are assigned dynamically */
#define STRING_MANUFACTURER_IDX        0
#define STRING_PRODUCT_IDX        1
#define STRING_SERIAL_IDX        2

static char manufacturer_string[256];
static char product_string[256];
static char serial_string[256];

/* String Table */
static struct usb_string strings_dev[] = {
    [STRING_MANUFACTURER_IDX].s = manufacturer_string,
    [STRING_PRODUCT_IDX].s = product_string,
    [STRING_SERIAL_IDX].s = serial_string,
    {  }            /* end of list */
};

static struct usb_gadget_strings stringtab_dev = {
    .language    = 0x0409,    /* en-us */
    .strings    = strings_dev,
};

static struct usb_gadget_strings *dev_strings[] = {
    &stringtab_dev,
    NULL,
};

static struct usb_device_descriptor device_desc = {
    .bLength              = sizeof(device_desc),
    .bDescriptorType      = USB_DT_DEVICE,
    .bcdUSB               = __constant_cpu_to_le16(0x0200),
    .bDeviceClass         = USB_CLASS_PER_INTERFACE,
    .idVendor             = __constant_cpu_to_le16(VENDOR_ID),
    .idProduct            = __constant_cpu_to_le16(PRODUCT_ID),
    .bcdDevice            = __constant_cpu_to_le16(0xffff),
    .bNumConfigurations   = 1,
};

static struct usb_configuration android_config_driver = {
    .label        = "android",
    .unbind        = android_unbind_config,
    .setup      = android_setup_config,
    .bConfigurationValue = 1,
    .bmAttributes    = USB_CONFIG_ATT_ONE | USB_CONFIG_ATT_SELFPOWER,
    .MaxPower    = 500, /* 500ma */
};

static struct usb_ctrlrequest android_ctrl_request;

ATOMIC_NOTIFIER_HEAD(android_usb_suspend_notifier_list);
ATOMIC_NOTIFIER_HEAD(android_usb_resume_notifier_list);

EXPORT_SYMBOL(android_usb_suspend_notifier_list);
EXPORT_SYMBOL(android_usb_resume_notifier_list);

static void android_work(struct work_struct *data)
{
    struct android_dev *dev = container_of(data, struct android_dev, work);
    struct usb_composite_dev *cdev = dev->cdev;
    char *disconnected[2] = { "USB_STATE=DISCONNECTED", NULL };
    char *connected[2]    = { "USB_STATE=CONNECTED", NULL };
    char *configured[2]   = { "USB_STATE=CONFIGURED", NULL };
    char *disabled[2]   = { "USB_STATE=SETPROP", NULL };
    char **uevent_envp = NULL;
    unsigned long flags;

    printk(KERN_DEBUG "%s +\n",__func__);

    if (!cdev) {
        printk(KERN_ERR "%s:the point cdev is NULL !\n",__func__);
        return;
    }

    spin_lock_irqsave(&cdev->lock, flags);
    if (dev->disabled)
        uevent_envp = disabled;
    else if (cdev->config)
        uevent_envp = configured;
    else if (dev->connected != dev->sw_connected)
        uevent_envp = dev->connected ? connected : disconnected;
    dev->sw_connected = dev->connected;
    spin_unlock_irqrestore(&cdev->lock, flags);

    if (uevent_envp) {
        if (uevent_envp != disabled) {
            kobject_uevent_env(&dev->dev->kobj, KOBJ_CHANGE, uevent_envp);
        }
        pr_info("%s: sent uevent %s\n", __func__, uevent_envp[0]);
    } else {
        pr_info("%s: did not send uevent (%d %d %p)\n", __func__,
             dev->connected, dev->sw_connected, cdev->config);
    }
    printk(KERN_DEBUG "%s -\n",__func__);
}

static void android_enable(struct android_dev *dev)
{
    struct usb_composite_dev *cdev = dev->cdev;

    printk(KERN_DEBUG "%s +\n",__func__);
    if (WARN_ON(!dev->disable_depth))
        return;
    printk(KERN_DEBUG "android device enable depth: %d\n", dev->disable_depth);
    if (--dev->disable_depth == 0) {
        printk(KERN_DEBUG "usb_add_config +\n");
        usb_add_config(cdev, &android_config_driver,
                    android_bind_config);
        printk(KERN_DEBUG "usb_gadget_connect +\n");
        usb_gadget_connect(cdev->gadget);
    }
    printk(KERN_DEBUG "%s -\n",__func__);
}

static void android_disable(struct android_dev *dev)
{
    struct usb_composite_dev *cdev = dev->cdev;

    printk(KERN_DEBUG "%s +\n",__func__);
    printk(KERN_DEBUG "android device disable depth: %d\n", dev->disable_depth);
    if (dev->disable_depth++ == 0) {
        gser_num = 0;
        rndis_num = 0;
        printk(KERN_DEBUG "usb_gadget_disconnect +\n");
        usb_gadget_disconnect(cdev->gadget);
        /* Cancel pending control requests */
        printk(KERN_DEBUG "usb_ep_dequeue +\n");
        usb_ep_dequeue(cdev->gadget->ep0, cdev->req);
        printk(KERN_DEBUG "usb_remove_config +\n");
        usb_remove_config(cdev, &android_config_driver);
	cdev->next_string_id = setup_string_id;
    }
    printk(KERN_DEBUG "%s -\n",__func__);
}

/*-------------------------------------------------------------------------*/
/* Supported functions initialization */

struct functionfs_config {
    bool opened;
    bool enabled;
    struct ffs_data *data;
};

static int ffs_function_init(struct android_usb_function *f,
                 struct usb_composite_dev *cdev)
{
    f->config = kzalloc(sizeof(struct functionfs_config), GFP_KERNEL);
    if (!f->config)
        return -ENOMEM;

    return functionfs_init();
}

static void ffs_function_cleanup(struct android_usb_function *f)
{
    functionfs_cleanup();
    kfree(f->config);
}

static void ffs_function_enable(struct android_usb_function *f)
{
    struct android_dev *dev = _android_dev;
    struct functionfs_config *config = f->config;

    printk(KERN_DEBUG "%s +\n",__func__);
    config->enabled = true;

    /* Disable the gadget until the function is ready */
    if (!config->opened)
        android_disable(dev);
    printk(KERN_DEBUG "%s -\n",__func__);
}

static void ffs_function_disable(struct android_usb_function *f)
{
    struct android_dev *dev = _android_dev;
    struct functionfs_config *config = f->config;

    printk(KERN_DEBUG "%s +\n",__func__);
    config->enabled = false;

    /* Balance the disable that was called in closed_callback */
    if (!config->opened)
        android_enable(dev);
    printk(KERN_DEBUG "%s -\n",__func__);
}

static int ffs_function_bind_config(struct android_usb_function *f,
                    struct usb_configuration *c)
{
    struct functionfs_config *config = f->config;
    return functionfs_bind_config(c->cdev, c, config->data);
}

static ssize_t
ffs_aliases_show(struct device *pdev, struct device_attribute *attr, char *buf)
{
    struct android_dev *dev = _android_dev;
    int ret;

    mutex_lock(&dev->mutex);
    ret = sprintf(buf, "%s\n", dev->ffs_aliases);
    mutex_unlock(&dev->mutex);

    return ret;
}

static ssize_t
ffs_aliases_store(struct device *pdev, struct device_attribute *attr,
                    const char *buf, size_t size)
{
    struct android_dev *dev = _android_dev;
    char buff[256];

    mutex_lock(&dev->mutex);

    if (dev->enabled) {
        mutex_unlock(&dev->mutex);
        return -EBUSY;
    }

    strlcpy(buff, buf, sizeof(buff));
    strlcpy(dev->ffs_aliases, strim(buff), sizeof(dev->ffs_aliases));

    mutex_unlock(&dev->mutex);

    return size;
}

static DEVICE_ATTR(aliases, S_IRUGO | S_IWUSR, ffs_aliases_show,
                           ffs_aliases_store);
static struct device_attribute *ffs_function_attributes[] = {
    &dev_attr_aliases,
    NULL
};

static struct android_usb_function ffs_function = {
    .name        = "ffs",
    .init        = ffs_function_init,
    .enable        = ffs_function_enable,
    .disable    = ffs_function_disable,
    .cleanup    = ffs_function_cleanup,
    .bind_config    = ffs_function_bind_config,
    .attributes    = ffs_function_attributes,
};

static int functionfs_ready_callback(struct ffs_data *ffs)
{
    struct android_dev *dev = _android_dev;
    struct functionfs_config *config = ffs_function.config;
    int ret = 0;

    printk(KERN_DEBUG "%s +\n",__func__);
    mutex_lock(&dev->mutex);

    ret = functionfs_bind(ffs, dev->cdev);
    if (ret)
        goto err;

    config->data = ffs;
    config->opened = true;

    if (config->enabled)
        android_enable(dev);
    printk(KERN_DEBUG "%s -\n",__func__);
err:
    mutex_unlock(&dev->mutex);
    return ret;
}

static void functionfs_closed_callback(struct ffs_data *ffs)
{
    struct android_dev *dev = _android_dev;
    struct functionfs_config *config = ffs_function.config;

    printk(KERN_DEBUG "%s +\n",__func__);
    mutex_lock(&dev->mutex);

    if (config->enabled)
        android_disable(dev);

    config->opened = false;
    config->data = NULL;

    functionfs_unbind(ffs);

    mutex_unlock(&dev->mutex);
    printk(KERN_DEBUG "%s -\n",__func__);
}

static void *functionfs_acquire_dev_callback(const char *dev_name)
{
    return 0;
}

static void functionfs_release_dev_callback(struct ffs_data *ffs_data)
{
}

struct adb_data {
    bool opened;
    bool enabled;
};

static int
adb_function_init(struct android_usb_function *f,
        struct usb_composite_dev *cdev)
{
    pr_info("android_usb: enter %s\n", __func__);
    f->config = kzalloc(sizeof(struct adb_data), GFP_KERNEL);
    if (!f->config)
        return -ENOMEM;

    return adb_setup();
}

static void adb_function_cleanup(struct android_usb_function *f)
{
    pr_info("android_usb: enter %s\n", __func__);
    adb_cleanup();
    kfree(f->config);
}

static int
adb_function_bind_config(struct android_usb_function *f,
        struct usb_configuration *c)
{
    pr_info("android_usb: enter %s\n", __func__);
    return adb_bind_config(c);
}

#if 0
static void adb_android_function_enable(struct android_usb_function *f)
{
    struct android_dev *dev = _android_dev;
    struct adb_data *data = f->config;

    printk(KERN_DEBUG "%s +\n",__func__);
    data->enabled = true;

    /* Disable the gadget until adbd is ready */

    if (!data->opened)
        android_disable(dev);

    printk(KERN_DEBUG "%s -\n",__func__);

}

static void adb_android_function_disable(struct android_usb_function *f)
{
    struct android_dev *dev = _android_dev;
    struct adb_data *data = f->config;

    printk(KERN_DEBUG "%s +\n",__func__);
    data->enabled = false;

    /* Balance the disable that was called in closed_callback */
    if (!data->opened)
        android_enable(dev);
    printk(KERN_DEBUG "%s -\n",__func__);
}
#endif /* if 0 @ line: static void adb_android_function_enable(struct android_usb_function *f) */

static struct android_usb_function adb_function = {
    .name        = "adb",
/*    .enable        = adb_android_function_enable, */
/*    .disable    = adb_android_function_disable, */
    .init        = adb_function_init,
    .cleanup    = adb_function_cleanup,
    .bind_config    = adb_function_bind_config,
};

#if 0
static void adb_ready_callback(void)
{
    struct android_dev *dev = _android_dev;
    struct adb_data *data = adb_function.config;

    printk(KERN_DEBUG "%s +\n",__func__);
    mutex_lock(&dev->mutex);

    data->opened = true;

    if (data->enabled)
        android_enable(dev);

    mutex_unlock(&dev->mutex);
    printk(KERN_DEBUG "%s -\n",__func__);
}

static void adb_closed_callback(void)
{
    struct android_dev *dev = _android_dev;
    struct adb_data *data = adb_function.config;

    printk(KERN_DEBUG "%s +\n",__func__);
    mutex_lock(&dev->mutex);

    data->opened = false;

    if (data->enabled)
        android_disable(dev);

    mutex_unlock(&dev->mutex);
    printk(KERN_DEBUG "%s -\n",__func__);
}
#endif  /* if 0 @ line: static void adb_ready_callback(void) */

struct hdb_data {
    bool opened;
    bool enabled;
};

static int
hdb_function_init(struct android_usb_function *f,
        struct usb_composite_dev *cdev)
{
    f->config = kzalloc(sizeof(struct hdb_data), GFP_KERNEL);
    if (!f->config)
        return -ENOMEM;

    return hdb_setup();
}

static void hdb_function_cleanup(struct android_usb_function *f)
{
    hdb_cleanup();
    kfree(f->config);
}

static int
hdb_function_bind_config(struct android_usb_function *f,
        struct usb_configuration *c)
{
    return hdb_bind_config(c);
}

#if 0
static void hdb_android_function_enable(struct android_usb_function *f)
{
    /* normal mode contains hdb port even if adbd is not started */
#ifndef CONFIG_HUAWEI_USB
    struct android_dev *dev = _android_dev;
    struct hdb_data *data = f->config;

    data->enabled = true;


    /* Disable the gadget until adbd is ready */
    if (!data->opened)
        android_disable(dev);
#endif
}

static void hdb_android_function_disable(struct android_usb_function *f)
{
    /* normal mode contains adb port even if adbd is not started */
#ifndef CONFIG_HUAWEI_USB
    struct android_dev *dev = _android_dev;
    struct hdb_data *data = f->config;

    data->enabled = false;

    /* Balance the disable that was called in closed_callback */
    if (!data->opened)
        android_enable(dev);
#endif
}
#endif

static struct android_usb_function hdb_function = {
    .name        = "hdb",
//    .enable      = hdb_android_function_enable,
//    .disable     = hdb_android_function_disable,
    .init        = hdb_function_init,
    .cleanup     = hdb_function_cleanup,
    .bind_config = hdb_function_bind_config,
};

#if 0
static void hdb_ready_callback(void)
{
    /* normal mode contains adb port even if adbd is not started */
#ifndef CONFIG_HUAWEI_USB
    struct android_dev *dev = _android_dev;
    struct hdb_data *data = hdb_function.config;

    /* dev is null in case ADB is not in the composition */
    if (dev)
        mutex_lock(&dev->mutex);

    /* Save dev in case the adb function will get disabled */
    //data->dev = dev;
    data->opened = true;

    if (data->enabled && dev)
        android_enable(dev);

    if (dev)
        mutex_unlock(&dev->mutex);
#endif
}

static void hdb_closed_callback(void)
{
    /* normal mode contains adb port even if adbd is not started */
#ifndef CONFIG_HUAWEI_USB
    struct hdb_data *data = hdb_function.config;
    struct android_dev *dev = _android_dev;

    /* In case new composition is without ADB, use saved one */
    //if (!dev)
    //dev = data->dev;

    if (!dev)
    pr_err("adb_closed_callback: data->dev is NULL");

    if (dev)
        mutex_lock(&dev->mutex);

    data->opened = false;

    if (data->enabled)
        android_disable(dev);

    //data->dev = NULL;

    if (dev)
        mutex_unlock(&dev->mutex);
#endif
}
#endif

static void acm_complete_40a2(struct usb_ep *ep,
		struct usb_request *req)
{
	pr_info(KERN_DEBUG "%s: +-\n", __func__);
}

static int acm_function_ctrlrequest(struct android_usb_function *f,
				struct usb_composite_dev *cdev,
				const struct usb_ctrlrequest *ctrl)
{
	int	value = -EOPNOTSUPP;
	u16	w_index = le16_to_cpu(ctrl->wIndex);
	u16	w_value = le16_to_cpu(ctrl->wValue);
	u16	w_length = le16_to_cpu(ctrl->wLength);

	switch ((ctrl->bRequestType << 8) | ctrl->bRequest) {
	case ((USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE) << 8) | 0xa2:
		pr_info("f_acm request: wValue: 0x%04x, wIndex: 0x%04x, wLength: 0x%04x\n",
			w_value, w_index, w_length);
		value = w_length;
		cdev->gadget->ep0->driver_data = cdev;
		cdev->req->complete = acm_complete_40a2;
		break;
	default:
		break;
	}

	/* respond with data transfer or status phase? */
	if (value >= 0) {
		int rc;
		pr_info("f_acm queue request\n");
		cdev->req->zero = value < w_length;
		cdev->req->length = value;
		rc = usb_ep_queue(cdev->gadget->ep0, cdev->req, GFP_ATOMIC);
		if (rc < 0)
			pr_err("%s: response queue error\n", __func__);
	}

	return value;
}

#define MAX_ACM_INSTANCES 7
struct acm_function_config {
    int instances;
    int instances_on;
    struct usb_function *f_acm[MAX_ACM_INSTANCES];
    struct usb_function_instance *f_acm_inst[MAX_ACM_INSTANCES];
};

static int
acm_function_init(struct android_usb_function *f,
        struct usb_composite_dev *cdev)
{
    int i;
    int ret;
    struct acm_function_config *config;

    pr_info("android_usb: enter %s\n", __func__);
    config = kzalloc(sizeof(struct acm_function_config), GFP_KERNEL);
    if (!config)
        return -ENOMEM;
    f->config = config;
    ((struct acm_function_config *)(f->config))->instances = MAX_ACM_INSTANCES; /* added by hisi-balong */
    for (i = 0; i < MAX_ACM_INSTANCES; i++) {
        config->f_acm_inst[i] = usb_get_function_instance("gser");
        if (IS_ERR(config->f_acm_inst[i])) {
            ret = PTR_ERR(config->f_acm_inst[i]);
            goto err_usb_get_function_instance;
        }
        config->f_acm[i] = usb_get_function(config->f_acm_inst[i]);
        if (IS_ERR(config->f_acm[i])) {
            ret = PTR_ERR(config->f_acm[i]);
            goto err_usb_get_function;
        }
    }
    return 0;
err_usb_get_function_instance:
    while (i-- > 0) {
        usb_put_function(config->f_acm[i]);
err_usb_get_function:
        usb_put_function_instance(config->f_acm_inst[i]);
    }
    return ret;
}

static void acm_function_cleanup(struct android_usb_function *f)
{
    int i;
    struct acm_function_config *config = f->config;
    pr_info("android_usb: enter %s\n", __func__);
    for (i = 0; i < MAX_ACM_INSTANCES; i++) {
        usb_put_function(config->f_acm[i]);
        usb_put_function_instance(config->f_acm_inst[i]);
    }
    kfree(f->config);
    f->config = NULL;
}

static int
acm_function_bind_config(struct android_usb_function *f,
        struct usb_configuration *c)
{
    int i;
    int ret = 0;
    struct acm_function_config *config = f->config;

    config->instances_on = config->instances;
    for (i = 0; i < config->instances_on; i++) {
        ret = usb_add_function(c, config->f_acm[i]);
        if (ret) {
            pr_err("Could not bind acm%u config\n", i);
            goto err_usb_add_function;
        }
    }

    return 0;

err_usb_add_function:
    while (i-- > 0)
        usb_remove_function(c, config->f_acm[i]);
    return ret;
}


#if 0
static void acm_function_unbind_config(struct android_usb_function *f,
                       struct usb_configuration *c)
{
    int i;
    struct acm_function_config *config = f->config;

    for (i = 0; i < config->instances_on; i++)
        usb_remove_function(c, config->f_acm[i]);
}
#endif

static ssize_t acm_instances_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
    struct android_usb_function *f = dev_get_drvdata(dev);
    struct acm_function_config *config = f->config;
    return sprintf(buf, "%d\n", config->instances);
}

static ssize_t acm_instances_store(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t size)
{
    struct android_usb_function *f = dev_get_drvdata(dev);
    struct acm_function_config *config = f->config;
    int value;

    sscanf(buf, "%d", &value);
    if (value > MAX_ACM_INSTANCES)
        value = MAX_ACM_INSTANCES;
    config->instances = value;
    return size;
}

static DEVICE_ATTR(instances, S_IRUGO | S_IWUSR, acm_instances_show,
                         acm_instances_store);
static struct device_attribute *acm_function_attributes[] = {
    &dev_attr_instances,
    NULL
};

static struct android_usb_function acm_function = {
    .name        = "acm",
    .init        = acm_function_init,
    .cleanup    = acm_function_cleanup,
    .bind_config    = acm_function_bind_config,
    //.unbind_config    = acm_function_unbind_config,
    .attributes    = acm_function_attributes,
	.ctrlrequest	= acm_function_ctrlrequest,
};

#ifdef CONFIG_USB_F_BALONG_ACM

#define MAX_BALONG_ACM_INSTANCES 8

struct balong_acm_function_config {
	struct usb_function *f_acm[MAX_BALONG_ACM_INSTANCES];
	struct usb_function_instance *f_acm_inst[MAX_BALONG_ACM_INSTANCES];
};

static int
balong_acm_function_init(struct android_usb_function *f,
		struct usb_composite_dev *cdev)
{
	int i;
	int ret;
	struct balong_acm_function_config *config;
D("+\n");

	config = kzalloc(sizeof(struct balong_acm_function_config), GFP_KERNEL);
	if (!config)
		return -ENOMEM;
	f->config = config;

	for (i = 0; i < MAX_BALONG_ACM_INSTANCES; i++) {
		config->f_acm_inst[i] = usb_get_function_instance("balong_acm");
		if (IS_ERR(config->f_acm_inst[i])) {
			ret = PTR_ERR(config->f_acm_inst[i]);
			goto err_usb_get_function_instance;
		}
		config->f_acm[i] = usb_get_function(config->f_acm_inst[i]);
		if (IS_ERR(config->f_acm[i])) {
			ret = PTR_ERR(config->f_acm[i]);
			goto err_usb_get_function;
		}
	}
D("-\n");
	return 0;

err_usb_get_function_instance:
	while (i-- > 0) {
		usb_put_function(config->f_acm[i]);
err_usb_get_function:
		usb_put_function_instance(config->f_acm_inst[i]);
	}
	return ret;
}

static void balong_acm_function_cleanup(struct android_usb_function *f)
{
	int i;
	struct balong_acm_function_config *config = f->config;

	for (i = 0; i < MAX_BALONG_ACM_INSTANCES; i++) {
		usb_put_function(config->f_acm[i]);
		usb_put_function_instance(config->f_acm_inst[i]);
	}
	kfree(f->config);
	f->config = NULL;
}

extern void bsp_usb_init_enum_stat(void);

static int
balong_acm_function_bind_config(struct android_usb_function *f,
		struct usb_configuration *c)
{
	int i;
	int ret = 0;
	struct balong_acm_function_config *config = f->config;
D("+\n");


	bsp_usb_init_enum_stat();


	for (i = 0; i < MAX_BALONG_ACM_INSTANCES; i++) {
		ret = usb_add_function(c, config->f_acm[i]);
		if (ret) {
			pr_err("Could not bind acm%u config\n", i);
			goto err_usb_add_function;
		}
	}
D("-\n");

	return 0;

err_usb_add_function:
	while (i-- > 0)
		usb_remove_function(c, config->f_acm[i]);
	return ret;
}

static struct android_usb_function balong_acm_function = {
	.name		= "balong_acm",
	.init		= balong_acm_function_init,
	.cleanup	= balong_acm_function_cleanup,
	.bind_config	= balong_acm_function_bind_config,
	.ctrlrequest	= acm_function_ctrlrequest,
};


#endif


static int
mtp_function_init(struct android_usb_function *f,
        struct usb_composite_dev *cdev)
{
    return mtp_setup();
}

static void mtp_function_cleanup(struct android_usb_function *f)
{
    mtp_cleanup();
}

static int
mtp_function_bind_config(struct android_usb_function *f,
        struct usb_configuration *c)
{
    return mtp_bind_config(c, false);
}

static int
ptp_function_init(struct android_usb_function *f,
        struct usb_composite_dev *cdev)
{
    /* nothing to do - initialization is handled by mtp_function_init */
    return 0;
}

static void ptp_function_cleanup(struct android_usb_function *f)
{
    /* nothing to do - cleanup is handled by mtp_function_cleanup */
}

static int
ptp_function_bind_config(struct android_usb_function *f,
        struct usb_configuration *c)
{
    return mtp_bind_config(c, true);
}

static int mtp_function_ctrlrequest(struct android_usb_function *f,
                    struct usb_composite_dev *cdev,
                    const struct usb_ctrlrequest *c)
{
    return mtp_ctrlrequest(cdev, c);
}

static struct android_usb_function mtp_function = {
    .name        = "mtp",
    .init        = mtp_function_init,
    .cleanup    = mtp_function_cleanup,
    .bind_config    = mtp_function_bind_config,
    .ctrlrequest    = mtp_function_ctrlrequest,
};

/* PTP function is same as MTP with slightly different interface descriptor */
static struct android_usb_function ptp_function = {
    .name        = "ptp",
    .init        = ptp_function_init,
    .cleanup    = ptp_function_cleanup,
    .bind_config    = ptp_function_bind_config,
    .ctrlrequest    = mtp_function_ctrlrequest,
};


struct rndis_function_config {
    u8      ethaddr[ETH_ALEN];
    u32     vendorID;
    char    manufacturer[256];
    /* "Wireless" RNDIS; auto-detected by Windows */
    bool    wceis;
    struct eth_dev *dev;
};

static int
rndis_function_init(struct android_usb_function *f,
        struct usb_composite_dev *cdev)
{
    f->config = kzalloc(sizeof(struct rndis_function_config), GFP_KERNEL);
    if (!f->config)
        return -ENOMEM;
    return 0;
}

static void rndis_function_cleanup(struct android_usb_function *f)
{
    kfree(f->config);
    f->config = NULL;
}

static int
rndis_function_bind_config(struct android_usb_function *f,
        struct usb_configuration *c)
{
    int ret;
    struct eth_dev *dev;
    struct rndis_function_config *rndis = f->config;

    if (!rndis) {
        pr_err("%s: rndis_pdata\n", __func__);
        return -1;
    }

    pr_info("%s MAC: %02X:%02X:%02X:%02X:%02X:%02X\n", __func__,
        rndis->ethaddr[0], rndis->ethaddr[1], rndis->ethaddr[2],
        rndis->ethaddr[3], rndis->ethaddr[4], rndis->ethaddr[5]);

    dev = gether_setup_name(c->cdev->gadget, rndis->ethaddr, "rndis");
    if (IS_ERR(dev)) {
        ret = PTR_ERR(dev);
        pr_err("%s: gether_setup failed\n", __func__);
        return ret;
    }
    rndis->dev = dev;

    if (rndis->wceis) {
        /* "Wireless" RNDIS; auto-detected by Windows */
        rndis_iad_descriptor.bFunctionClass =
                        USB_CLASS_WIRELESS_CONTROLLER;
        rndis_iad_descriptor.bFunctionSubClass = 0x01;
        rndis_iad_descriptor.bFunctionProtocol = 0x03;
        rndis_control_intf.bInterfaceClass =
                        USB_CLASS_WIRELESS_CONTROLLER;
        rndis_control_intf.bInterfaceSubClass =     0x01;
        rndis_control_intf.bInterfaceProtocol =     0x03;
    }

    return rndis_bind_config_vendor(c, rndis->ethaddr, rndis->vendorID,
                       rndis->manufacturer, rndis->dev);
}

static void rndis_function_unbind_config(struct android_usb_function *f,
                        struct usb_configuration *c)
{
    struct rndis_function_config *rndis = f->config;
    gether_cleanup(rndis->dev);
}

static ssize_t rndis_manufacturer_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
    struct android_usb_function *f = dev_get_drvdata(dev);
    struct rndis_function_config *config = f->config;
    return sprintf(buf, "%s\n", config->manufacturer);
}

static ssize_t rndis_manufacturer_store(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t size)
{
    struct android_usb_function *f = dev_get_drvdata(dev);
    struct rndis_function_config *config = f->config;

    if (size >= sizeof(config->manufacturer))
        return -EINVAL;
    if (sscanf(buf, "%s", config->manufacturer) == 1)
        return size;
    return -1;
}

static DEVICE_ATTR(manufacturer, S_IRUGO | S_IWUSR, rndis_manufacturer_show,
                            rndis_manufacturer_store);

static ssize_t rndis_wceis_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
    struct android_usb_function *f = dev_get_drvdata(dev);
    struct rndis_function_config *config = f->config;
    return sprintf(buf, "%d\n", config->wceis);
}

static ssize_t rndis_wceis_store(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t size)
{
    struct android_usb_function *f = dev_get_drvdata(dev);
    struct rndis_function_config *config = f->config;
    int value;

    if (sscanf(buf, "%d", &value) == 1) {
        config->wceis = value;
        return size;
    }
    return -EINVAL;
}

static DEVICE_ATTR(wceis, S_IRUGO | S_IWUSR, rndis_wceis_show,
                         rndis_wceis_store);

static ssize_t rndis_ethaddr_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
    struct android_usb_function *f = dev_get_drvdata(dev);
    struct rndis_function_config *rndis = f->config;
    return sprintf(buf, "%02x:%02x:%02x:%02x:%02x:%02x\n",
        rndis->ethaddr[0], rndis->ethaddr[1], rndis->ethaddr[2],
        rndis->ethaddr[3], rndis->ethaddr[4], rndis->ethaddr[5]);
}

static ssize_t rndis_ethaddr_store(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t size)
{
    struct android_usb_function *f = dev_get_drvdata(dev);
    struct rndis_function_config *rndis = f->config;

    if (sscanf(buf, "%02x:%02x:%02x:%02x:%02x:%02x\n",
            (int *)&rndis->ethaddr[0], (int *)&rndis->ethaddr[1],
            (int *)&rndis->ethaddr[2], (int *)&rndis->ethaddr[3],
            (int *)&rndis->ethaddr[4], (int *)&rndis->ethaddr[5]) == 6)
        return size;
    return -EINVAL;
}

static DEVICE_ATTR(ethaddr, S_IRUGO | S_IWUSR, rndis_ethaddr_show,
                           rndis_ethaddr_store);

static ssize_t rndis_vendorID_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
    struct android_usb_function *f = dev_get_drvdata(dev);
    struct rndis_function_config *config = f->config;
    return sprintf(buf, "%04x\n", config->vendorID);
}

static ssize_t rndis_vendorID_store(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t size)
{
    struct android_usb_function *f = dev_get_drvdata(dev);
    struct rndis_function_config *config = f->config;
    int value;

    if (sscanf(buf, "%04x", &value) == 1) {
        config->vendorID = value;
        return size;
    }
    return -EINVAL;
}

static DEVICE_ATTR(vendorID, S_IRUGO | S_IWUSR, rndis_vendorID_show,
                        rndis_vendorID_store);

static struct device_attribute *rndis_function_attributes[] = {
    &dev_attr_manufacturer,
    &dev_attr_wceis,
    &dev_attr_ethaddr,
    &dev_attr_vendorID,
    NULL
};

static struct android_usb_function rndis_function = {
    .name        = "rndis",
    .init        = rndis_function_init,
    .cleanup    = rndis_function_cleanup,
    .bind_config    = rndis_function_bind_config,
    .unbind_config    = rndis_function_unbind_config,
    .attributes    = rndis_function_attributes,
};


struct mass_storage_function_config {
    struct fsg_config fsg;
    struct fsg_common *common;
};

#define MS_STG_SET_LEN         (32)
#define FSG_MAX_LUNS_HUAWEI    (2)
static char autorun[MS_STG_SET_LEN] = "enable";        /* enable/disable autorun function "enable"/"disable" */
static char luns[MS_STG_SET_LEN]    = "sdcard";        /* "sdcard"/"cdrom,sdcard"/"cdrom"/"sdcard,cdrom" can be used*/

static int mass_storage_function_init(struct android_usb_function *f,
                    struct usb_composite_dev *cdev)
{
    struct mass_storage_function_config *config;
    struct fsg_common *common;
    int err;
        char *name = NULL;
        char buf[MS_STG_SET_LEN];
        char *b = NULL;
        memset(buf, 0, sizeof(buf));
    config = kzalloc(sizeof(struct mass_storage_function_config),
                                GFP_KERNEL);
    pr_info("android_usb: enter %s\n", __func__);
    if (!config){
        pr_err("%s: %d failed\n", __func__, __LINE__);
        return -ENOMEM;
    }

    strlcpy(buf, luns, sizeof(buf));
    b = strim(buf);
    config->fsg.nluns = 0;
    while (b){
            name = strsep(&b, ",");
            if(name){
                if(0==strcmp(name, "sdcard")){
                config->fsg.luns[config->fsg.nluns].cdrom = 0;
                config->fsg.luns[config->fsg.nluns].ro = 0;
                config->fsg.luns[config->fsg.nluns].removable = 1;
                config->fsg.luns[config->fsg.nluns].nofua = 0;
                config->fsg.nluns++;
                pr_info("mass_storage: %s, %d\n", name, config->fsg.nluns);
            }else if(0==strcmp(name, "cdrom")){
                    if(0==strcmp(autorun, "disable") ){
                        continue;
                }else if(0!=strcmp(autorun, "enable")){
                        pr_err("mass_storage: not support autorun type '%s' \n", autorun);
                        continue;
                }
                config->fsg.luns[config->fsg.nluns].cdrom = 1;
                config->fsg.luns[config->fsg.nluns].ro = 1;
                config->fsg.luns[config->fsg.nluns].removable = 1;
                config->fsg.luns[config->fsg.nluns].nofua = 1;
                config->fsg.nluns++;
                pr_info("mass_storage: %s, %d\n", name, config->fsg.nluns);
            }else{
                pr_err("mass_storage: not support storage type '%s' \n", name);
            }
        }
        if(config->fsg.nluns >= FSG_MAX_LUNS_HUAWEI){
                        pr_err("mass_storage: not support too many luns\n");
            break;
        }
    }

    common = fsg_common_init(NULL, cdev, &config->fsg);
    if (IS_ERR(common)) {
        kfree(config);
        pr_err("%s: %d failed\n", __func__, __LINE__);
        return PTR_ERR(common);
    }

    err = sysfs_create_link(&f->dev->kobj,
                &common->luns[0].dev.kobj,
                "lun");
    if (err) {
        kfree(config);
        pr_err("%s: %d failed\n", __func__, __LINE__);
        return err;
    }

    if(config->fsg.nluns > 1 ){
        err = sysfs_create_link(&f->dev->kobj,&common->luns[1].dev.kobj, "lun1");
        if (err)
        {
            sysfs_delete_link(&f->dev->kobj,&common->luns[0].dev.kobj, "lun");
            kfree(config);
            pr_err("%s: %d failed\n", __func__, __LINE__);
            return err;
        }
    }

    config->common = common;
    f->config = config;
    return 0;
}

static void mass_storage_function_cleanup(struct android_usb_function *f)
{
    pr_info("android_usb: enter %s\n", __func__);
    kfree(f->config);
    f->config = NULL;
}

static int mass_storage_function_bind_config(struct android_usb_function *f,
                        struct usb_configuration *c)
{
    struct mass_storage_function_config *config = f->config;
    pr_info("android_usb: enter %s\n", __func__);
    return fsg_bind_config(c->cdev, c, config->common);
}

static ssize_t mass_storage_inquiry_show(struct device *dev,
                struct device_attribute *attr, char *buf)
{
    struct android_usb_function *f = dev_get_drvdata(dev);
    struct mass_storage_function_config *config = f->config;
    return sprintf(buf, "%s\n", config->common->inquiry_string);
}

static ssize_t mass_storage_inquiry_store(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t size)
{
    struct android_usb_function *f = dev_get_drvdata(dev);
    struct mass_storage_function_config *config = f->config;
    if (size >= sizeof(config->common->inquiry_string))
        return -EINVAL;
    if (sscanf(buf, "%s", config->common->inquiry_string) != 1)
        return -EINVAL;
    return size;
}

static DEVICE_ATTR(inquiry_string, S_IRUGO | S_IWUSR,
                    mass_storage_inquiry_show,
                    mass_storage_inquiry_store);

#include "hw_msconfig.c"
#include "hw_hisuite.c"

static struct device_attribute *mass_storage_function_attributes[] = {
    &dev_attr_inquiry_string,
    &dev_attr_autorun,
    &dev_attr_luns,
    &dev_attr_suitestate,
    NULL
};

static struct android_usb_function mass_storage_function = {
    .name        = "mass_storage",
    .init        = mass_storage_function_init,
    .cleanup    = mass_storage_function_cleanup,
    .bind_config    = mass_storage_function_bind_config,
    .attributes    = mass_storage_function_attributes,
};


static int accessory_function_init(struct android_usb_function *f,
                    struct usb_composite_dev *cdev)
{
    pr_info("android_usb: enter %s\n", __func__);
    return acc_setup();
}

static void accessory_function_cleanup(struct android_usb_function *f)
{
    pr_info("android_usb: enter %s\n", __func__);
    acc_cleanup();
}

static int accessory_function_bind_config(struct android_usb_function *f,
                        struct usb_configuration *c)
{
    pr_info("android_usb: enter %s\n", __func__);
    return acc_bind_config(c);
}

static int accessory_function_ctrlrequest(struct android_usb_function *f,
                        struct usb_composite_dev *cdev,
                        const struct usb_ctrlrequest *c)
{
    pr_info("android_usb: enter %s\n", __func__);
    return acc_ctrlrequest(cdev, c);
}

static struct android_usb_function accessory_function = {
    .name        = "accessory",
    .init        = accessory_function_init,
    .cleanup    = accessory_function_cleanup,
    .bind_config    = accessory_function_bind_config,
    .ctrlrequest    = accessory_function_ctrlrequest,
};

static int audio_source_function_init(struct android_usb_function *f,
            struct usb_composite_dev *cdev)
{
    struct audio_source_config *config;

    config = kzalloc(sizeof(struct audio_source_config), GFP_KERNEL);
    if (!config)
        return -ENOMEM;
    config->card = -1;
    config->device = -1;
    f->config = config;
    return 0;
}

static void audio_source_function_cleanup(struct android_usb_function *f)
{
    kfree(f->config);
}

static int audio_source_function_bind_config(struct android_usb_function *f,
                        struct usb_configuration *c)
{
    struct audio_source_config *config = f->config;

    return audio_source_bind_config(c, config);
}

static void audio_source_function_unbind_config(struct android_usb_function *f,
                        struct usb_configuration *c)
{
    struct audio_source_config *config = f->config;

    config->card = -1;
    config->device = -1;
}

static ssize_t audio_source_pcm_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
    struct android_usb_function *f = dev_get_drvdata(dev);
    struct audio_source_config *config = f->config;

    /* print PCM card and device numbers */
    return sprintf(buf, "%d %d\n", config->card, config->device);
}

static DEVICE_ATTR(pcm, S_IRUGO, audio_source_pcm_show, NULL);

static struct device_attribute *audio_source_function_attributes[] = {
    &dev_attr_pcm,
    NULL
};

static struct android_usb_function audio_source_function = {
    .name        = "audio_source",
    .init        = audio_source_function_init,
    .cleanup    = audio_source_function_cleanup,
    .bind_config    = audio_source_function_bind_config,
    .unbind_config    = audio_source_function_unbind_config,
    .attributes    = audio_source_function_attributes,
};

#include "hw_acm_functions.c"
#include "hw_modem_serial.c"

static struct android_usb_function *supported_functions[] = {
    &ffs_function,
    &adb_function,
    &hdb_function,
    &acm_function,
    &hw_acm_function,
    &mtp_function,
    &ptp_function,
    &rndis_function,
    &mass_storage_function,
    &accessory_function,
    &audio_source_function,

#ifdef CONFIG_USB_F_SERIAL
    &nmea_function,
    &modem_function,
    &diag_function,
#endif

#ifdef CONFIG_USB_F_BALONG_ACM
	&balong_acm_function,
#endif

    NULL
};


static int android_init_functions(struct android_usb_function **functions,
                  struct usb_composite_dev *cdev)
{
    struct android_dev *dev = _android_dev;
    struct android_usb_function *f;
    struct device_attribute **attrs;
    struct device_attribute *attr;
    int err = 0;
    int index = 0;

    for (; (f = *functions++); index++) {
        f->dev_name = kasprintf(GFP_KERNEL, "f_%s", f->name);
        f->dev = device_create(android_class, dev->dev,
                MKDEV(0, index), f, f->dev_name);
        if (IS_ERR(f->dev)) {
            pr_err("%s: Failed to create dev %s", __func__,
                            f->dev_name);
            err = PTR_ERR(f->dev);
            goto err_create;
        }

        if (f->init) {
            err = f->init(f, cdev);
            if (err) {
                pr_err("%s: Failed to init %s", __func__,
                                f->name);
                goto err_out;
            }
        }

        attrs = f->attributes;
        if (attrs) {
            while ((attr = *attrs++) && !err)
                err = device_create_file(f->dev, attr);
        }
        if (err) {
            pr_err("%s: Failed to create function %s attributes",
                    __func__, f->name);
            goto err_out;
        }
/*move usb_shell_init to usb_shell.c*/
#if 0
#ifdef CONFIG_HISILICON_PLATFORM_USB
        /* added by hisi-balong*/
        /* if it's acm device, just kick off usb shell */
        if (!strcmp(f->name, "acm")) {
            /* VID and PID supported now */
            if (__constant_cpu_to_le16(0x12d1) == device_desc.idVendor) {
                if ((__constant_cpu_to_le16(0x3a13) == device_desc.idProduct) ||
                    (__constant_cpu_to_le16(0x1057) == device_desc.idProduct)) {
                    /*usb_shell_init();*/
                }
            }
        }
        /* added by hisi-balong */
#endif
#endif
    }
    return 0;

err_out:
    device_destroy(android_class, f->dev->devt);
err_create:
    kfree(f->dev_name);
    return err;
}

static void android_cleanup_functions(struct android_usb_function **functions)
{
    struct android_usb_function *f;

    while (*functions) {
        f = *functions++;

        if (f->dev) {
            device_destroy(android_class, f->dev->devt);
            kfree(f->dev_name);
        }

        if (f->cleanup)
            f->cleanup(f);
    }
}

static int
android_bind_enabled_functions(struct android_dev *dev,
                   struct usb_configuration *c)
{
    struct android_usb_function *f;
    int ret;

    printk(KERN_DEBUG "%s +\n",__func__);
    list_for_each_entry(f, &dev->enabled_functions, enabled_list) {
        ret = f->bind_config(f, c);
        if (ret) {
            pr_err("%s: %s failed", __func__, f->name);
            return ret;
        }
    }
    printk(KERN_DEBUG "%s -\n",__func__);
    return 0;
}

static void
android_unbind_enabled_functions(struct android_dev *dev,
                   struct usb_configuration *c)
{
    struct android_usb_function *f;

    printk(KERN_DEBUG "%s +\n",__func__);
    list_for_each_entry(f, &dev->enabled_functions, enabled_list) {
        if (f->unbind_config)
            f->unbind_config(f, c);
    }
    printk(KERN_DEBUG "%s -\n",__func__);
}

static int android_enable_function(struct android_dev *dev, char *name)
{
    struct android_usb_function **functions = dev->functions;
    struct android_usb_function *f;
    while ((f = *functions++)) {
        if (!strcmp(name, f->name)) {
            list_add_tail(&f->enabled_list,
                        &dev->enabled_functions);
            return 0;
        }
    }
    return -EINVAL;
}

/*-------------------------------------------------------------------------*/
/* /sys/class/android_usb/android%d/ interface */

static ssize_t
functions_show(struct device *pdev, struct device_attribute *attr, char *buf)
{
    struct android_dev *dev = dev_get_drvdata(pdev);
    struct android_usb_function *f;
    char *buff = buf;

    mutex_lock(&dev->mutex);

    list_for_each_entry(f, &dev->enabled_functions, enabled_list)
        buff += sprintf(buff, "%s,", f->name);

    mutex_unlock(&dev->mutex);

    if (buff != buf)
        *(buff-1) = '\n';
    return buff - buf;
}

static ssize_t
functions_store(struct device *pdev, struct device_attribute *attr,
                   const char *buff, size_t size)
{
    struct android_dev *dev = dev_get_drvdata(pdev);
    char *name;
    char buf[256], *b;
    char aliases[256], *a;
    int err;
    int is_ffs;
    int ffs_enabled = 0;

    mutex_lock(&dev->mutex);

    if (dev->enabled) {
        mutex_unlock(&dev->mutex);
        return -EBUSY;
    }

    INIT_LIST_HEAD(&dev->enabled_functions);

    strlcpy(buf, buff, sizeof(buf));
    b = strim(buf);

    while (b) {
        name = strsep(&b, ",");
        pr_info("android_usb: enable '%s'\n", name);
        if (!name)
            continue;

        is_ffs = 0;
        strlcpy(aliases, dev->ffs_aliases, sizeof(aliases));
        a = aliases;

        while (a) {
            char *alias = strsep(&a, ",");
            if (alias && !strcmp(name, alias)) {
                is_ffs = 1;
                break;
            }
        }

        if (is_ffs) {
            if (ffs_enabled)
                continue;
            err = android_enable_function(dev, "ffs");
            if (err)
                pr_err("android_usb: Cannot enable ffs (%d)",
                                    err);
            else
                ffs_enabled = 1;
            continue;
        }

        err = android_enable_function(dev, name);
        if (err)
            pr_err("android_usb: Cannot enable '%s' (%d)",
                               name, err);
    }

    mutex_unlock(&dev->mutex);

    return size;
}

static ssize_t enable_show(struct device *pdev, struct device_attribute *attr,
               char *buf)
{
    struct android_dev *dev = dev_get_drvdata(pdev);
    printk(KERN_DEBUG "%s +\n",__func__);
    return sprintf(buf, "%d\n", dev->enabled);
}

static ssize_t enable_store(struct device *pdev, struct device_attribute *attr,
                const char *buff, size_t size)
{
    struct android_dev *dev = dev_get_drvdata(pdev);
    struct usb_composite_dev *cdev = dev->cdev;
    struct android_usb_function *f;
    int enabled = 0;

    printk(KERN_DEBUG "%s +\n",__func__);
    if (!cdev){
        printk(KERN_ERR "%s cdev is NULL!\n",__func__);
        return -ENODEV;
    }
    mutex_lock(&dev->mutex);

    printk(KERN_DEBUG "%s %d buff is 0x%p\n", __FUNCTION__,__LINE__,buff);
    sscanf(buff, "%d", &enabled);
    printk(KERN_DEBUG "%s %d enabled is %d\n", __FUNCTION__,__LINE__,enabled);
    if (!enabled) {
        port_num = 0;
        gser_num = 0;
        rndis_num = 0;
       }
    if (!strcmp(serial_string, "cleanup")) {
        cdev->desc.iSerialNumber = 0;
        memset(serial_string, 0, sizeof(serial_string));
    } else if (!strcmp(serial_string, "")) {
        cdev->desc.iSerialNumber = device_desc.iSerialNumber;
        strncpy(serial_string, "0123456789ABCDEF", sizeof(serial_string) - 1);
    } else {
        cdev->desc.iSerialNumber = device_desc.iSerialNumber;
    }

    printk(KERN_DEBUG "%s %d dev->enabled is %d\n", __FUNCTION__,__LINE__,dev->enabled);
    if (enabled && !dev->enabled) {
        printk(KERN_DEBUG "%s %d android_enable \n", __FUNCTION__,__LINE__);
        /*
         * Update values in composite driver's copy of
         * device descriptor.
         */
        cdev->desc.idVendor = device_desc.idVendor;
        cdev->desc.idProduct = device_desc.idProduct;
        cdev->desc.bcdDevice = device_desc.bcdDevice;
        cdev->desc.bDeviceClass = device_desc.bDeviceClass;
        cdev->desc.bDeviceSubClass = device_desc.bDeviceSubClass;
        cdev->desc.bDeviceProtocol = device_desc.bDeviceProtocol;
        list_for_each_entry(f, &dev->enabled_functions, enabled_list) {
            if (f->enable)
                f->enable(f);
        }
        android_enable(dev);
        dev->enabled = true;

    } else if (!enabled && dev->enabled) {
        printk(KERN_DEBUG "%s %d android_disable \n", __FUNCTION__,__LINE__);

	/* close all stored file in each lun */
	fsg_close_all_file(((struct mass_storage_function_config *)mass_storage_function.config)->common);

        android_disable(dev);
        list_for_each_entry(f, &dev->enabled_functions, enabled_list) {
            if (f->disable)
                f->disable(f);
        }

        if(android_acm_debug_check())
        {
            /* Disconnect for usb setprop */
            atomic_notifier_call_chain(&android_usb_suspend_notifier_list, 0, 0);

            dev->disabled = 1;
            dev->event = USB_DEVIEC_PLUGOUT;
            schedule_work(&dev->notify_work);
            do
            {
                msleep(10);
            }while(dev->disabled == 1);
        }

        dev->enabled = false;
    } else {
        pr_err("android_usb: already %s\n",
                dev->enabled ? "enabled" : "disabled");
    }

    mutex_unlock(&dev->mutex);
    printk(KERN_DEBUG "%s -\n",__func__);
    return size;
}

static ssize_t portNum_show(struct device *pdev, struct device_attribute *attr,
                       char *buf)
{
        return sprintf(buf, "%d\n", port_num);
}

static ssize_t portNum_store(struct device *pdev, struct device_attribute *attr,
                        const char *buf, size_t size)
{
     int value;
     if (sscanf(buf, "%04x\n", &value) == 1) {
         port_num = value;
         printk(KERN_INFO "port_num: %04x\n", value);
         return size;
     }
     return -1;
}

static ssize_t state_show(struct device *pdev, struct device_attribute *attr,
               char *buf)
{
    struct android_dev *dev = dev_get_drvdata(pdev);
    struct usb_composite_dev *cdev = dev->cdev;
    char *state = "DISCONNECTED";
    unsigned long flags;

    if (!cdev)
        goto out;

    spin_lock_irqsave(&cdev->lock, flags);
    if (cdev->config)
        state = "CONFIGURED";
    else if (dev->connected)
        state = "CONNECTED";
    spin_unlock_irqrestore(&cdev->lock, flags);
out:
    return sprintf(buf, "%s\n", state);
}

#define DESCRIPTOR_ATTR(field, format_string)                \
static ssize_t                                \
field ## _show(struct device *dev, struct device_attribute *attr,    \
        char *buf)                        \
{                                    \
    return sprintf(buf, format_string, device_desc.field);        \
}                                    \
static ssize_t                                \
field ## _store(struct device *dev, struct device_attribute *attr,    \
        const char *buf, size_t size)                \
{                                    \
    int value;                            \
    if (sscanf(buf, format_string, &value) == 1) {            \
        device_desc.field = value;                \
        return size;                        \
    }                                \
    return -1;                            \
}                                    \
static DEVICE_ATTR(field, S_IRUGO | S_IWUSR, field ## _show, field ## _store);

#define DESCRIPTOR_STRING_ATTR(field, buffer)                \
static ssize_t                                \
field ## _show(struct device *dev, struct device_attribute *attr,    \
        char *buf)                        \
{                                    \
    return sprintf(buf, "%s", buffer);                \
}                                    \
static ssize_t                                \
field ## _store(struct device *dev, struct device_attribute *attr,    \
        const char *buf, size_t size)                \
{                                    \
    if (size >= sizeof(buffer))                    \
        return -EINVAL;                        \
    if (sscanf(buf, "%s", buffer) == 1) {                \
        return size;                        \
    }                                \
    return -1;                            \
}                                    \
static DEVICE_ATTR(field, S_IRUGO | S_IWUSR, field ## _show, field ## _store);


DESCRIPTOR_ATTR(idVendor, "%04x\n")
DESCRIPTOR_ATTR(idProduct, "%04x\n")
DESCRIPTOR_ATTR(bcdDevice, "%04x\n")
DESCRIPTOR_ATTR(bDeviceClass, "%d\n")
DESCRIPTOR_ATTR(bDeviceSubClass, "%d\n")
DESCRIPTOR_ATTR(bDeviceProtocol, "%d\n")
DESCRIPTOR_STRING_ATTR(iManufacturer, manufacturer_string)
DESCRIPTOR_STRING_ATTR(iProduct, product_string)
DESCRIPTOR_STRING_ATTR(iSerial, serial_string)

static DEVICE_ATTR(functions, S_IRUGO | S_IWUSR, functions_show,
                         functions_store);
static DEVICE_ATTR(enable, S_IRUGO | S_IWUSR, enable_show, enable_store);
static DEVICE_ATTR(state, S_IRUGO, state_show, NULL);
static DEVICE_ATTR(portNum, S_IRUGO | S_IWUSR, portNum_show, portNum_store);
/* read the attribute to indentify if there is a switch request has been sent or not
 * write 0 to clear the request flag
 */

#include "hw_rwswitch.c"

static struct device_attribute *android_usb_attributes[] = {
    &dev_attr_idVendor,
    &dev_attr_idProduct,
    &dev_attr_bcdDevice,
    &dev_attr_bDeviceClass,
    &dev_attr_bDeviceSubClass,
    &dev_attr_bDeviceProtocol,
    &dev_attr_iManufacturer,
    &dev_attr_iProduct,
    &dev_attr_iSerial,
    &dev_attr_functions,
    &dev_attr_enable,
    &dev_attr_state,
    &dev_attr_portNum,
    &dev_attr_switch_request,
    &dev_attr_port_mode,
    &dev_attr_switch_index,
    NULL
};
#ifdef HW_USB_TIME_SYNC_PC
    #define AFTER_BOOT_TO_1970YEAR ((1971-1970)*365*24*60*60)
    #define BEIJING_TIME_ZONE 8

struct gFlush_PC_Data {
    struct timespec tv;
	struct delayed_work pc_data_work;
};

struct gFlush_PC_Data flush_pc_data;

static void flushPcDataWork(struct work_struct *work)
{
    struct gFlush_PC_Data *pc_data = container_of(work,
            struct gFlush_PC_Data, pc_data_work.work);
    struct rtc_device *rtc;
    struct rtc_time new_rtc_tm;
    int rv = 0;

    if(do_settimeofday(&pc_data->tv) < 0)
    {
        printk(KERN_INFO "[USB_DEBUG]set system time Fail!\n");
    }
    rtc = rtc_class_open(CONFIG_RTC_HCTOSYS_DEVICE);
    if (rtc == NULL) {
        printk(KERN_INFO "%s: unable to open rtc device (%s)\n",
                __FILE__, CONFIG_RTC_HCTOSYS_DEVICE);
        return;
    }

    rtc_time_to_tm(pc_data->tv.tv_sec, &new_rtc_tm);
    rv = rtc_set_time(rtc, &new_rtc_tm);
    if(rv!=0)
    {
        printk(KERN_INFO "[USB_DEBUG]set RTC time Fail!\n");
    }
    printk(KERN_INFO "[USB_DEBUG]set system time ok!\n");
    //	spin_lock_irqsave(&alarm_slock, flags);
    //	alarm_pending |= ANDROID_ALARM_TIME_CHANGE_MASK;
    //	wake_up(&alarm_wait_queue);
    //	spin_unlock_irqrestore(&alarm_slock, flags);
}
#endif
/*-------------------------------------------------------------------------*/
/* Composite driver */
static void android_handle_host_time(struct usb_ep *ep, struct usb_request *req)
{
    struct _time *host_time;
#ifdef HW_USB_TIME_SYNC_PC
    struct timeval tvNow;
#endif

    host_time = (struct _time *)req->buf;

    printk(KERN_INFO "Time Bias:%d minutes\n", host_time->time_bias);

    printk(KERN_INFO "Host Time:[%d:%d:%d %d:%d:%d:%d Weekday:%d]\n", host_time->year,
            host_time->month, host_time->day, host_time->hour, host_time->minute,
            host_time->second, host_time->millisecond, host_time->weekday);
#ifdef HW_USB_TIME_SYNC_PC
    do_gettimeofday(&tvNow);
    if(AFTER_BOOT_TO_1970YEAR < (tvNow.tv_sec))
        return;

    flush_pc_data.tv.tv_nsec = NSEC_PER_SEC >> 1;
    flush_pc_data.tv.tv_sec = (unsigned long) mktime (host_time->year,
                    host_time->month,
                    host_time->day,
                    (host_time->hour+BEIJING_TIME_ZONE),
                    host_time->minute,
                    host_time->second);
    schedule_delayed_work(&(flush_pc_data.pc_data_work), 0);
#endif
    return;
}

static int android_bind_config(struct usb_configuration *c)
{
    struct android_dev *dev = _android_dev;
    int ret = 0;

    printk(KERN_INFO "%s +\n",__func__);

    ret = android_bind_enabled_functions(dev, c);
    if (ret)
        return ret;
    printk(KERN_INFO "%s -\n",__func__);
    return 0;
}

static void android_unbind_config(struct usb_configuration *c)
{
    struct android_dev *dev = _android_dev;

    android_unbind_enabled_functions(dev, c);
}

static int android_setup_config(struct usb_configuration *c,
        const struct usb_ctrlrequest *ctrl)
{
    struct usb_composite_dev *cdev = c->cdev;
    struct usb_request *req = cdev->req;
    u16     w_index  = le16_to_cpu(ctrl->wIndex);
    u16     w_value  = le16_to_cpu(ctrl->wValue);
    u16     w_length = le16_to_cpu(ctrl->wLength);
    int     value    = -EOPNOTSUPP;

    /*
     * for MBB spec command such like "40 A2",
     * to support any other commands, add here.
     */
    switch (ctrl->bRequestType) {
    case (USB_DIR_OUT | USB_TYPE_VENDOR):
        switch (ctrl->bRequest) {
        case (USB_REQ_SEND_HOST_TIME):
            /*
             * "40 A1"-The host sends sys-time to device
             */
            memcpy((void *)&android_ctrl_request, (void *)ctrl,
                    sizeof(struct usb_ctrlrequest));
            req->context = &android_ctrl_request;
            req->complete = android_handle_host_time;
            cdev->gadget->ep0->driver_data = cdev;
            value = w_length;
            break;

        default:
            pr_warning("invalid control req%02x.%02x v%04x i%04x l%d\n",
                ctrl->bRequestType, ctrl->bRequest,    w_value, w_index, w_length);
            break;
        }
        break;

    default:
        pr_warning("invalid control req%02x.%02x v%04x i%04x l%d\n",
            ctrl->bRequestType, ctrl->bRequest,    w_value, w_index, w_length);
        break;
    }

    /* respond with data transfer or status phase? */
    if (value >= 0) {
        req->zero = 0;
        req->length = value;
        value = usb_ep_queue(cdev->gadget->ep0, req, GFP_ATOMIC);
        if (value < 0) {
            pr_warning("android_setup_config response err 0x%x\n", value);
        }
    }

    return value;
}

static int android_bind(struct usb_composite_dev *cdev)
{
    struct android_dev *dev = _android_dev;
    struct usb_gadget    *gadget = cdev->gadget;
    int            id, ret;

    printk(KERN_INFO "%s +\n",__func__);
    /*
     * Start disconnected. Userspace will connect the gadget once
     * it is done configuring the functions.
     */
    usb_gadget_disconnect(gadget);

    ret = android_init_functions(dev->functions, cdev);
    if (ret)
        return ret;

    /* Allocate string descriptor numbers ... note that string
     * contents can be overridden by the composite_dev glue.
     */
    id = usb_string_id(cdev);
    if (id < 0)
        return id;
    strings_dev[STRING_MANUFACTURER_IDX].id = id;
    device_desc.iManufacturer = id;

    id = usb_string_id(cdev);
    if (id < 0)
        return id;
    strings_dev[STRING_PRODUCT_IDX].id = id;
    device_desc.iProduct = id;

    /* Default strings - should be updated by userspace */
    strncpy(manufacturer_string, "Android", sizeof(manufacturer_string)-1);
    strncpy(product_string, "Android", sizeof(product_string) - 1);
    strncpy(serial_string, "0123456789ABCDEF", sizeof(serial_string) - 1);

    id = usb_string_id(cdev);
    if (id < 0)
        return id;
    strings_dev[STRING_SERIAL_IDX].id = id;
    device_desc.iSerialNumber = id;
	setup_string_id = cdev->next_string_id;
    device_desc.bcdDevice = cpu_to_le16(USB_DEVICE_BCD_CODE);

#ifdef CONFIG_USB_F_BALONG_ACM
	device_desc.bcdDevice =
			cpu_to_le16(0x0200 | 0x0099);
#endif

    usb_gadget_set_selfpowered(gadget);
    dev->cdev = cdev;
    printk(KERN_INFO "%s -\n",__func__);
    return 0;
}

static int android_usb_unbind(struct usb_composite_dev *cdev)
{
    struct android_dev *dev = _android_dev;

    printk(KERN_INFO "[%s]cancel android_work\n", __func__);
    cancel_work_sync(&dev->work);
    cancel_work_sync(&dev->notify_work);
    android_cleanup_functions(dev->functions);
    return 0;
}

/* HACK: android needs to override setup for accessory to work */
static int (*composite_setup_func)(struct usb_gadget *gadget, const struct usb_ctrlrequest *c);

static void android_usb_suspend(struct usb_composite_dev *cdev)
{
    pr_info("%s: enter\n", __func__);

    if (android_acm_debug_check()) {
        atomic_notifier_call_chain(&android_usb_suspend_notifier_list, 0, 0);
    }

    return;
}

static void android_usb_resume(struct usb_composite_dev *cdev)
{
    /*atomic_notifier_call_chain(&android_usb_resume_notifier_list, 0, 0);*/
    return;
}
static int
android_setup(struct usb_gadget *gadget, const struct usb_ctrlrequest *c)
{
    struct android_dev        *dev = _android_dev;
    struct usb_composite_dev    *cdev = NULL;
    struct usb_request        *req = NULL;
    struct android_usb_function    *f;
    int value = -EOPNOTSUPP;
    unsigned long flags;
    #if defined (CONFIG_HUAWEI_DSM)
    int dsm_usb_error_found = -1;

    if (!dsm_client_ocuppy(usb_dclient)) {
		dsm_usb_error_found++;
	}
    #endif
	
    cdev = get_gadget_data(gadget);
    if (!cdev) {
        printk(KERN_ERR "the point cdev is NULL !\n");
        return -1;
    }
    req = cdev->req;
    req->zero = 0;
    req->length = 0;
    gadget->ep0->driver_data = cdev;

    list_for_each_entry(f, &dev->enabled_functions, enabled_list) {
        if (f->ctrlrequest) {
            value = f->ctrlrequest(f, cdev, c);
            if (value >= 0)
                break;
        }
    }

    /* Special case the accessory function.
     * It needs to handle control requests before it is enabled.
     */
    if (value < 0)
        value = acc_ctrlrequest(cdev, c);

    if (value < 0)
        value = composite_setup_func(gadget, c);

    spin_lock_irqsave(&cdev->lock, flags);
    if (!dev->connected) {
        dev->connected = 1;
        printk(KERN_INFO "[%s]schedule android_work with connect\n", __func__);
        schedule_work(&dev->work);
        pr_info("android_setup connected\n");
    } else if (c->bRequest == USB_REQ_SET_CONFIGURATION &&
                        cdev->config) {
        printk(KERN_INFO "[%s]schedule android_work with configured\n", __func__);
        schedule_work(&dev->work);

        dev->event = USB_DEVIEC_PLUGIN;
        schedule_work(&dev->notify_work);
        pr_info("android_setup configured\n");
		
	#if defined (CONFIG_HUAWEI_DSM)
	USB_DSM(dsm_usb_error_found, "[%s]schedule android_work with configured\n", 
                            __func__);
	if (dsm_usb_error_found > 0) {
        dsm_client_notify(usb_dclient, DSM_USB_ERROR_NO);
        } else if (!dsm_usb_error_found) {
    	dsm_client_unocuppy(usb_dclient);
       }
       #endif
    }
    spin_unlock_irqrestore(&cdev->lock, flags);
    return value;
}

static void android_disconnect(struct usb_composite_dev *cdev)
{
    struct android_dev *dev = _android_dev;
    #if defined (CONFIG_HUAWEI_DSM)
    int dsm_usb_error_found = -1;

    if (!dsm_client_ocuppy(usb_dclient)) {
		dsm_usb_error_found++;
	}
    #endif

    printk(KERN_INFO "%s +\n",__func__);

    if (!dev) {
        printk(KERN_ERR "%s:the point dev is NULL !\n",__func__);
        return;
    }

    /* for usb invalid such like unplugged */
    if (android_acm_debug_check()) {
        atomic_notifier_call_chain(&android_usb_suspend_notifier_list, 0, 0);
    }

    dev->connected = 0;
    schedule_work(&dev->work);

    dev->event = USB_DEVIEC_PLUGOUT;
    schedule_work(&dev->notify_work);

    /* accessory HID support can be active while the
       accessory function is not actually enabled,
       so we need to inform it when we are disconnected.
     */
    acc_disconnect();

	#if defined (CONFIG_HUAWEI_DSM)
	USB_DSM(dsm_usb_error_found, "[%s]android_disconnect \n", 
                            __func__);
	if (dsm_usb_error_found > 0) {
        dsm_client_notify(usb_dclient, DSM_USB_ERROR_NO);
        } else if (!dsm_usb_error_found) {
    	dsm_client_unocuppy(usb_dclient);
       }
       #endif

    pr_info("android_disconnect \n");
    printk(KERN_INFO "%s -\n",__func__);
}

static struct usb_composite_driver android_usb_driver = {
    .name        = "android_usb",
    .dev        = &device_desc,
    .strings    = dev_strings,
    .bind        = android_bind,
    .unbind        = android_usb_unbind,
    .disconnect    = android_disconnect,
    .max_speed    = USB_SPEED_HIGH,
    .suspend    = android_usb_suspend,
    .resume     = android_usb_resume,

};

static int android_create_device(struct android_dev *dev)
{
    struct device_attribute **attrs = android_usb_attributes;
    struct device_attribute *attr;
    int err;

    dev->dev = device_create(android_class, NULL,
                    MKDEV(0, 0), NULL, "android0");
    if (IS_ERR(dev->dev))
        return PTR_ERR(dev->dev);

    dev_set_drvdata(dev->dev, dev);

    while ((attr = *attrs++)) {
        err = device_create_file(dev->dev, attr);
        if (err) {
            device_destroy(android_class, dev->dev->devt);
            return err;
        }
    }
    return 0;
}

static void android_notify(struct work_struct *data)
{
#ifdef CONFIG_HISILICON_PLATFORM_USB
    struct android_dev *dev = _android_dev;
    pr_info("android_notify %d enter\n",dev->event);

    if (!android_acm_debug_check()) {
        pr_warning("android_notify acm not exist [%d] \n", dev->event);
        return;
    }

    if (USB_DEVIEC_PLUGIN == dev->event)
    {
        BSP_USB_UdiEnableNotify();
        atomic_notifier_call_chain(&android_usb_resume_notifier_list, 0, 0);
    }
    else if(USB_DEVIEC_PLUGOUT == dev->event)
    {
        BSP_USB_UdiDisableNotify();
        dev->disabled = 0;
    }
    else
    {
        printk(KERN_INFO "android_notify event[%d] error\n",dev->event);
    }
    pr_info("android_notify leave [%d]\n",dev->disabled);
#endif
}

static int __init init(void)
{
    struct android_dev *dev;
    int err;

    printk(KERN_INFO "%s +\n",__func__);
    android_class = class_create(THIS_MODULE, "android_usb");
    if (IS_ERR(android_class))
        return PTR_ERR(android_class);

    dev = kzalloc(sizeof(*dev), GFP_KERNEL);
    if (!dev) {
        err = -ENOMEM;
        goto err_dev;
    }

    dev->disable_depth = 1;
    dev->functions = supported_functions;
    INIT_LIST_HEAD(&dev->enabled_functions);
    INIT_WORK(&dev->work, android_work);
    mutex_init(&dev->mutex);

    INIT_WORK(&dev->notify_work, android_notify);
    dev->event = 0;
#ifdef HW_USB_TIME_SYNC_PC
    memset(&flush_pc_data, 0, sizeof(struct gFlush_PC_Data) );
    INIT_DELAYED_WORK(&(flush_pc_data.pc_data_work), flushPcDataWork);
#endif

    err = android_create_device(dev);
    if (err) {
        pr_err("%s: failed to create android device %d", __func__, err);
        goto err_create;
    }

    dev->disabled = 0;
    _android_dev = dev;

    err = usb_composite_probe(&android_usb_driver);
    if (err) {
        pr_err("%s: failed to probe driver %d", __func__, err);
        goto err_probe;
    }

    /* HACK: exchange composite's setup with ours */
    composite_setup_func = android_usb_driver.gadget_driver.setup;
    android_usb_driver.gadget_driver.setup = android_setup;

    printk(KERN_INFO "%s -\n",__func__);
    #if defined (CONFIG_HUAWEI_DSM)
	if (!usb_dclient) {
		usb_dclient = dsm_register_client(&dsm_usb);
	}
    #endif

    return 0;

err_probe:
    device_destroy(android_class, dev->dev->devt);
err_create:
    kfree(dev);
err_dev:
    class_destroy(android_class);
    return err;
}

module_init(init);

static void __exit cleanup(void)
{
    usb_composite_unregister(&android_usb_driver);
    class_destroy(android_class);
    kfree(_android_dev);
    _android_dev = NULL;
}
module_exit(cleanup);

/* added by hisi-balong */
/**
 * android_enable_function_check() - check whether the usb function exists
 * @f_name: the function name
 * Return of '1' means the function exist, 0 means non-exist .
 */
static int android_enable_function_check(unsigned char *f_name)
{
    struct android_dev *dev = _android_dev;
    struct android_usb_function *f;

    printk(KERN_INFO "%s +\n",__func__);
    list_for_each_entry(f, &dev->enabled_functions, enabled_list)
    {
        if (!strcmp(f->name, f_name))
        {
            return 1;
        }
    }
    printk(KERN_INFO "%s -\n",__func__);
    return 0;
}

/**
 * android_acm_debug_check() - check whether the usb is in debug mode, that
 * means all acm ports are used for debug and follow our own rules but tty's
 * Return of '1' means in debug mode, 0 but others .
 */
static int android_acm_debug_check(void)
{
    /**
    * besides the acm, the vid and pid must be the specific one
    */
    if (android_enable_function_check("acm")) {
        if (__constant_cpu_to_le16(0x12d1) == device_desc.idVendor) {
            if ((__constant_cpu_to_le16(0x3a13) == device_desc.idProduct) ||
                (__constant_cpu_to_le16(0x1057) == device_desc.idProduct)) {
                return 1;
            }
        }
    }
    return 0;
}

