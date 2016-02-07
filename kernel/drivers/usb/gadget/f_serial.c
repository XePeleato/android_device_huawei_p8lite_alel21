/*
 * f_serial.c - generic USB serial function driver
 *
 * Copyright (C) 2003 Al Borchers (alborchers@steinerpoint.com)
 * Copyright (C) 2008 by David Brownell
 * Copyright (C) 2008 by Nokia Corporation
 *
 * This software is distributed under the terms of the GNU General
 * Public License ("GPL") as published by the Free Software Foundation,
 * either version 2 of that License or (at your option) any later version.
 */

#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>

#include "u_serial.h"
#include "gadget_chips.h"
//#include <linux/hisi/drv_usb_if.h>

/*
 * This function packages a simple "generic serial" port with no real
 * control mechanisms, just raw data transfer over two bulk endpoints.
 *
 * Because it's not standardized, this isn't as interoperable as the
 * CDC ACM driver.  However, for many purposes it's just as functional
 * if you can arrange appropriate host side drivers.
 */

struct f_gser {
	struct gserial			port;
	u8				data_id;
	u8				port_num;
#ifdef CONFIG_USB_F_SERIAL
	struct usb_cdc_line_coding	port_line_coding;
	u16				port_handshake_bits;
#endif
};

static inline struct f_gser *func_to_gser(struct usb_function *f)
{
	return container_of(f, struct f_gser, port.func);
}

/*-------------------------------------------------------------------------*/

/* interface descriptor: */

static struct usb_interface_descriptor gser_interface_desc = {
	.bLength =		USB_DT_INTERFACE_SIZE,
	.bDescriptorType =	USB_DT_INTERFACE,
	/* .bInterfaceNumber = DYNAMIC */
	.bNumEndpoints =	2,
	.bInterfaceClass =	USB_CLASS_VENDOR_SPEC,
	.bInterfaceSubClass =	USB_CLASS_VENDOR_SPEC,
	.bInterfaceProtocol =	USB_CLASS_VENDOR_SPEC,
	/* .iInterface = DYNAMIC */
};

/* full speed support: */

static struct usb_endpoint_descriptor gser_fs_in_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,
	.bEndpointAddress =	USB_DIR_IN,
	.bmAttributes =		USB_ENDPOINT_XFER_BULK,
};

static struct usb_endpoint_descriptor gser_fs_out_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,
	.bEndpointAddress =	USB_DIR_OUT,
	.bmAttributes =		USB_ENDPOINT_XFER_BULK,
};

static struct usb_descriptor_header *gser_fs_function[] = {
	(struct usb_descriptor_header *) &gser_interface_desc,
	(struct usb_descriptor_header *) &gser_fs_in_desc,
	(struct usb_descriptor_header *) &gser_fs_out_desc,
	NULL,
};

/* high speed support: */

static struct usb_endpoint_descriptor gser_hs_in_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,
	.bmAttributes =		USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize =	cpu_to_le16(512),
};

static struct usb_endpoint_descriptor gser_hs_out_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,
	.bmAttributes =		USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize =	cpu_to_le16(512),
};

static struct usb_descriptor_header *gser_hs_function[] = {
	(struct usb_descriptor_header *) &gser_interface_desc,
	(struct usb_descriptor_header *) &gser_hs_in_desc,
	(struct usb_descriptor_header *) &gser_hs_out_desc,
	NULL,
};

static struct usb_endpoint_descriptor gser_ss_in_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,
	.bmAttributes =		USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize =	cpu_to_le16(1024),
};

static struct usb_endpoint_descriptor gser_ss_out_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,
	.bmAttributes =		USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize =	cpu_to_le16(1024),
};

static struct usb_ss_ep_comp_descriptor gser_ss_bulk_comp_desc = {
	.bLength =              sizeof gser_ss_bulk_comp_desc,
	.bDescriptorType =      USB_DT_SS_ENDPOINT_COMP,
};

static struct usb_descriptor_header *gser_ss_function[] = {
	(struct usb_descriptor_header *) &gser_interface_desc,
	(struct usb_descriptor_header *) &gser_ss_in_desc,
	(struct usb_descriptor_header *) &gser_ss_bulk_comp_desc,
	(struct usb_descriptor_header *) &gser_ss_out_desc,
	(struct usb_descriptor_header *) &gser_ss_bulk_comp_desc,
	NULL,
};

/* string descriptors: */

static struct usb_string gser_string_defs[] = {
	[0].s = "Generic Serial",
	{  } /* end of list */
};

static struct usb_gadget_strings gser_string_table = {
	.language =		0x0409,	/* en-us */
	.strings =		gser_string_defs,
};

static struct usb_gadget_strings *gser_strings[] = {
	&gser_string_table,
	NULL,
};

static struct usb_ctrlrequest gser_ctrl_request;

extern int BSP_ACM_CmdHandle(int cmd, int tty_idx, char *buf, int size);

/*-------------------------------------------------------------------------*/
static void gser_request_complete(struct usb_ep *ep, struct usb_request *req)
{
    struct usb_ctrlrequest *ctrl = (struct usb_ctrlrequest *)req->context;
    struct f_gser *f_gs = ep->driver_data;
    struct usb_composite_dev *cdev = f_gs->port.func.config->cdev;
    int ret= 0;
#ifdef CONFIG_HISILICON_PLATFORM_UDI
    ret = BSP_ACM_CmdHandle(ctrl->bRequest, f_gs->port_num,
                req->buf, req->length);
#endif
    if (ret < 0) {
        ERROR(cdev, "ttyGS%d handle command[0x%x] fail ret[0x%x]\n",
                f_gs->port_num, ctrl->bRequest, ret);
    }
    return;
}

static int gser_set_alt(struct usb_function *f, unsigned intf, unsigned alt)
{
	struct f_gser		*gser = func_to_gser(f);
	struct usb_composite_dev *cdev = f->config->cdev;

	/* we know alt == 0, so this is an activation or a reset */

	if (gser->port.in->driver_data) {
		DBG(cdev, "reset generic ttyGS%d\n", gser->port_num);
		gserial_disconnect(&gser->port);
	}
	if (!gser->port.in->desc || !gser->port.out->desc) {
		DBG(cdev, "activate generic ttyGS%d\n", gser->port_num);
		if (config_ep_by_speed(cdev->gadget, f, gser->port.in) ||
		    config_ep_by_speed(cdev->gadget, f, gser->port.out)) {
			gser->port.in->desc = NULL;
			gser->port.out->desc = NULL;
			return -EINVAL;
		}
	}
	gserial_connect(&gser->port, gser->port_num);
	return 0;
}

static void gser_disable(struct usb_function *f)
{
	struct f_gser	*gser = func_to_gser(f);
	struct usb_composite_dev *cdev = f->config->cdev;

	DBG(cdev, "generic ttyGS%d deactivated\n", gser->port_num);
	gserial_disconnect(&gser->port);
}

/*-------------------------------------------------------------------------*/

/* serial function driver setup/binding */

static int gser_bind(struct usb_configuration *c, struct usb_function *f)
{
	struct usb_composite_dev *cdev = c->cdev;
	struct f_gser		*gser = func_to_gser(f);
	int			status;
	struct usb_ep		*ep;

	/* REVISIT might want instance-specific strings to help
	 * distinguish instances ...
	 */

	/* maybe allocate device-global string ID */
	if (gser_string_defs[0].id == 0) {
		status = usb_string_id(c->cdev);
		if (status < 0)
			return status;
		gser_string_defs[0].id = status;
	}

	/* allocate instance-specific interface IDs */
	status = usb_interface_id(c, f);
	if (status < 0)
		goto fail;
	gser->data_id = status;
	gser_interface_desc.bInterfaceNumber = status;

	status = -ENODEV;

	/* allocate instance-specific endpoints */
	ep = usb_ep_autoconfig(cdev->gadget, &gser_fs_in_desc);
	if (!ep)
		goto fail;
	gser->port.in = ep;
	ep->driver_data = cdev;	/* claim */

	ep = usb_ep_autoconfig(cdev->gadget, &gser_fs_out_desc);
	if (!ep)
		goto fail;
	gser->port.out = ep;
	ep->driver_data = cdev;	/* claim */

	/* support all relevant hardware speeds... we expect that when
	 * hardware is dual speed, all bulk-capable endpoints work at
	 * both speeds
	 */
	gser_hs_in_desc.bEndpointAddress = gser_fs_in_desc.bEndpointAddress;
	gser_hs_out_desc.bEndpointAddress = gser_fs_out_desc.bEndpointAddress;

	gser_ss_in_desc.bEndpointAddress = gser_fs_in_desc.bEndpointAddress;
	gser_ss_out_desc.bEndpointAddress = gser_fs_out_desc.bEndpointAddress;

	status = usb_assign_descriptors(f, gser_fs_function, gser_hs_function,
			gser_ss_function);
	if (status)
		goto fail;
	DBG(cdev, "generic ttyGS%d: %s speed IN/%s OUT/%s\n",
			gser->port_num,
			gadget_is_superspeed(c->cdev->gadget) ? "super" :
			gadget_is_dualspeed(c->cdev->gadget) ? "dual" : "full",
			gser->port.in->name, gser->port.out->name);
    printk(KERN_INFO "generic ttyGS%d: %s speed IN/%s OUT/%s\n",
			gser->port_num,
			gadget_is_superspeed(c->cdev->gadget) ? "super" :
			gadget_is_dualspeed(c->cdev->gadget) ? "dual" : "full",
			gser->port.in->name, gser->port.out->name);
	return 0;

fail:
	/* we might as well release our claims on endpoints */
	if (gser->port.out)
		gser->port.out->driver_data = NULL;
	if (gser->port.in)
		gser->port.in->driver_data = NULL;

	ERROR(cdev, "%s: can't bind, err %d\n", f->name, status);

	return status;
}

static inline struct f_serial_opts *to_f_serial_opts(struct config_item *item)
{
	return container_of(to_config_group(item), struct f_serial_opts,
			    func_inst.group);
}

CONFIGFS_ATTR_STRUCT(f_serial_opts);
static ssize_t f_serial_attr_show(struct config_item *item,
				  struct configfs_attribute *attr,
				  char *page)
{
	struct f_serial_opts *opts = to_f_serial_opts(item);
	struct f_serial_opts_attribute *f_serial_opts_attr =
		container_of(attr, struct f_serial_opts_attribute, attr);
	ssize_t ret = 0;

	if (f_serial_opts_attr->show)
		ret = f_serial_opts_attr->show(opts, page);

	return ret;
}

static void serial_attr_release(struct config_item *item)
{
	struct f_serial_opts *opts = to_f_serial_opts(item);

	usb_put_function_instance(&opts->func_inst);
}

static struct configfs_item_operations serial_item_ops = {
	.release	= serial_attr_release,
	.show_attribute = f_serial_attr_show,
};

static ssize_t f_serial_port_num_show(struct f_serial_opts *opts, char *page)
{
	return sprintf(page, "%u\n", opts->port_num);
}

static struct f_serial_opts_attribute f_serial_port_num =
	__CONFIGFS_ATTR_RO(port_num, f_serial_port_num_show);

static struct configfs_attribute *acm_attrs[] = {
	&f_serial_port_num.attr,
	NULL,
};

static struct config_item_type serial_func_type = {
	.ct_item_ops	= &serial_item_ops,
	.ct_attrs	= acm_attrs,
	.ct_owner	= THIS_MODULE,
};

static void gser_free_inst(struct usb_function_instance *f)
{
	struct f_serial_opts *opts;

	opts = container_of(f, struct f_serial_opts, func_inst);
	gserial_free_line(opts->port_num);
	kfree(opts);
}

static struct usb_function_instance *gser_alloc_inst(void)
{
	struct f_serial_opts *opts;
	int ret;

	opts = kzalloc(sizeof(*opts), GFP_KERNEL);
	if (!opts)
		return ERR_PTR(-ENOMEM);

	opts->func_inst.free_func_inst = gser_free_inst;
	ret = gserial_alloc_line(&opts->port_num);
	if (ret) {
		kfree(opts);
		return ERR_PTR(ret);
	}
	config_group_init_type_name(&opts->func_inst.group, "",
				    &serial_func_type);

	return &opts->func_inst;
}

static void gser_free(struct usb_function *f)
{
	struct f_gser *serial;

	serial = func_to_gser(f);
	kfree(serial);
}

#ifdef CONFIG_USB_F_SERIAL
static void gser_complete_set_line_coding(struct usb_ep *ep,
		struct usb_request *req)
{
	struct f_gser            *gser = ep->driver_data;
	struct usb_composite_dev *cdev = gser->port.func.config->cdev;

	if (req->status != 0) {
		DBG(cdev, "gser ttyGS%d completion, err %d\n",
				gser->port_num, req->status);
        return;
	}

	/* normal completion */
	if (req->actual != sizeof(gser->port_line_coding)) {
		DBG(cdev, "gser ttyGS%d short resp, len %d\n",
				gser->port_num, req->actual);
		usb_ep_set_halt(ep);
	} else {
		struct usb_cdc_line_coding	*value = req->buf;
		gser->port_line_coding = *value;
	}
}


static int
gser_setup(struct usb_function *f, const struct usb_ctrlrequest *ctrl)
{
	struct f_gser            *gser = func_to_gser(f);
	struct usb_composite_dev *cdev = f->config->cdev;
	struct usb_request	 *req = cdev->req;
	int			 value = -EOPNOTSUPP;
	u16			 w_index = le16_to_cpu(ctrl->wIndex);
	u16			 w_value = le16_to_cpu(ctrl->wValue);
	u16			 w_length = le16_to_cpu(ctrl->wLength);

	printk(KERN_INFO  "gser ttyGS%d req%02x.%02x v%04x i%04x l%d\n",
			gser->port_num, ctrl->bRequestType, ctrl->bRequest,
			w_value, w_index, w_length);
#if 1
	if ((ctrl->bRequestType == 0x21) && (ctrl->bRequest == 0x02)) {
		value = 2;
		memset(req->buf, 0 , value);
	}

	if ((ctrl->bRequestType == 0x41) && (ctrl->bRequest == 0xa3)) {
		value = 1;
		memset(req->buf, 0 , value);
	}

	if ((ctrl->bRequestType == 0x01) && (ctrl->bRequest == 0x81)) {
		value = 1;
		memset(req->buf, 0 , value);
	}
#endif

	switch (ctrl->bRequestType) {

	case (USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE):
		switch(ctrl->bRequest){
			/* SET_LINE_CODING ... just read and save what the host sends */
			case USB_CDC_REQ_SET_LINE_CODING:
				if (w_length != sizeof(struct usb_cdc_line_coding))
					goto invalid;

				value = w_length;
				cdev->gadget->ep0->driver_data = gser;
				req->complete = gser_complete_set_line_coding;
				break;
			/* SET_CONTROL_LINE_STATE ... save what the host sent */
			case USB_CDC_REQ_SET_CONTROL_LINE_STATE:
				value = 0;
				gser->port_handshake_bits = w_value;
				if (gser->port.notify_modem) {
					unsigned port_num = gser->port_num;
					gser->port.notify_modem(&gser->port, port_num, w_value);
				}
				break;
			default:
				break;
		}
		break;

	case (USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE):
		switch(ctrl->bRequest){
			/* GET_LINE_CODING ... return what host sent, or initial value */
			case USB_CDC_REQ_GET_LINE_CODING:
				value = min_t(unsigned, w_length,
				sizeof(struct usb_cdc_line_coding));
				memcpy(req->buf, &gser->port_line_coding, value);
				break;
			default:
				break;
		}
		break;

    case (USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_INTERFACE):
        memcpy((void *)&gser_ctrl_request, (void *)ctrl,
                sizeof(struct usb_ctrlrequest));
        value = w_length;
        cdev->gadget->ep0->driver_data = gser;
        req->complete = gser_request_complete;
        req->context = &gser_ctrl_request;
        break;

    case (USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_INTERFACE):
        /* just respose with a zlp now, any other requirement, fix it here
         */
        value = 0;
        break;

	default:
invalid:
		DBG(cdev, "invalid control req%02x.%02x v%04x i%04x l%d\n",
			ctrl->bRequestType, ctrl->bRequest,
			w_value, w_index, w_length);
	}

	/* respond with data transfer or status phase? */
	if (value >= 0) {
		DBG(cdev, "gser ttyGS%d req%02x.%02x v%04x i%04x l%d\n",
			gser->port_num, ctrl->bRequestType, ctrl->bRequest,
			w_value, w_index, w_length);
		req->zero = 0;
		req->length = value;
		value = usb_ep_queue(cdev->gadget->ep0, req, GFP_ATOMIC);
		if (value < 0)
			ERROR(cdev, "gser response on ttyGS%d, err %d\n",
					gser->port_num, value);
	}

	/* device either stalls (value < 0) or reports success */
	return value;
}
#endif

static void gser_unbind(struct usb_configuration *c, struct usb_function *f)
{
	usb_free_all_descriptors(f);
}

struct usb_function *gser_alloc(struct usb_function_instance *fi)
{
	struct f_gser	*gser;
	struct f_serial_opts *opts;

	/* allocate and initialize one new instance */
	gser = kzalloc(sizeof(*gser), GFP_KERNEL);
	if (!gser)
		return ERR_PTR(-ENOMEM);

	opts = container_of(fi, struct f_serial_opts, func_inst);

	gser->port_num = opts->port_num;

	gser->port.func.name = "gser";
	gser->port.func.strings = gser_strings;
	gser->port.func.bind = gser_bind;
	gser->port.func.unbind = gser_unbind;
	gser->port.func.set_alt = gser_set_alt;
	gser->port.func.disable = gser_disable;
	gser->port.func.free_func = gser_free;
#ifdef CONFIG_USB_F_SERIAL
	gser->port.func.setup = gser_setup;
#endif
	return &gser->port.func;
}

/**
 * gser_bind_config - add a generic serial function to a configuration
 * @c: the configuration to support the serial instance
 * @port_num: /dev/ttyGS* port this interface will use
 * Context: single threaded during gadget setup
 *
 * Returns zero on success, else negative errno.
 *
 * Caller must have called @gserial_setup() with enough ports to
 * handle all the ones it binds.  Caller is also responsible
 * for calling @gserial_cleanup() before module unload.
 */
int gser_bind_config(struct usb_configuration *c, u8 port_num)
{
    struct f_gser   *gser;
    int     status;

    /* REVISIT might want instance-specific strings to help
     * distinguish instances ...
    */

    /* maybe allocate device-global string ID */
    if (gser_string_defs[0].id == 0) {
        status = usb_string_id(c->cdev);
        if (status < 0){
            printk(KERN_ERR "%s status %d\n",__func__,status);
            return status;
        }
        gser_string_defs[0].id = status;
    }

    /* allocate and initialize one new instance */
    gser = kzalloc(sizeof *gser, GFP_KERNEL);
    if (!gser)
        return -ENOMEM;

    gser->port_num = port_num;
    gser->port.func.name = "gser";

    gser->port.func.strings = gser_strings;
    gser->port.func.bind = gser_bind;
    gser->port.func.unbind = gser_unbind;
    gser->port.func.set_alt = gser_set_alt;
    gser->port.func.disable = gser_disable;
    gser->port.func.free_func = gser_free;
#ifdef CONFIG_USB_F_SERIAL
    gser->port.func.setup = gser_setup;
#endif

    status = usb_add_function(c, &gser->port.func);
    if (status)
        kfree(gser);

    return status;
}

DECLARE_USB_FUNCTION_INIT(gser, gser_alloc_inst, gser_alloc);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Al Borchers");
MODULE_AUTHOR("David Brownell");
