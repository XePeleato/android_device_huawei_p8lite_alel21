/*
 * linux/driver/usb/serial/hisi_serial.c
 *
 * Copyright (c) 2011 Hisi technology corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation;version 2 of the License.
 *
 * NOTE:Only use this driver for devices that provide AT/Diag port and all other generic serials.
 * and CDC-ACM port should probably be driven by option.ko.
 */

#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/module.h>
#include <linux/usb.h>
#include <linux/usb/serial.h>
#include <linux/slab.h>
#include "usb-wwan.h"

#define DRIVER_AUTHOR "Hisi Inc"
#define DRIVER_DESC "Hisi USB Serial driver"


static const struct usb_device_id id_table[] = {
	{USB_DEVICE(0x058b, 0x0041)},	/*  */
	{USB_DEVICE(0x12d1, 0x1506)},
	{ }				/* Terminating entry */
};
MODULE_DEVICE_TABLE(usb, id_table);


static int hisi_probe(struct usb_serial *serial, const struct usb_device_id *id)
{
	dev_info(&serial->dev->dev, "hisi serial probe\n");
	return 0;
}

static int hisi_attach(struct usb_serial *serial)
{
	struct usb_wwan_intf_private *data;

	data = kzalloc(sizeof(*data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	spin_lock_init(&data->susp_lock);

	usb_set_serial_data(serial, data);

	return 0;
}

static void hisi_release(struct usb_serial *serial)
{
	struct usb_wwan_intf_private *priv = usb_get_serial_data(serial);

	usb_set_serial_data(serial, NULL);
	kfree(priv);
}

static struct usb_serial_driver hisidevice = {
	.driver = {
		.owner     = THIS_MODULE,
		.name      = "hisi serial",
	},
	.description         = "Hisi USB modem",
	.id_table            = id_table,
	.num_ports           = 1,
	.probe               = hisi_probe,
	.open		     = usb_wwan_open,
	.close		     = usb_wwan_close,
	.write		     = usb_wwan_write,
	.write_room	     = usb_wwan_write_room,
	.chars_in_buffer     = usb_wwan_chars_in_buffer,
	.attach              = hisi_attach,
	.release	     = hisi_release,
	.port_probe          = usb_wwan_port_probe,
	.port_remove	     = usb_wwan_port_remove,
#ifdef CONFIG_PM
	.suspend	     = usb_wwan_suspend,
	.resume		     = usb_wwan_resume,
#endif
};

static struct usb_serial_driver * const serial_drivers[] = {
	&hisidevice, NULL
};

module_usb_serial_driver(serial_drivers, id_table);

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL v2");
