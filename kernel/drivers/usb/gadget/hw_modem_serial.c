/*
 * huawei modem and serial driver
 *
 */

#ifdef CONFIG_USB_F_SERIAL
#define PORT_NUM_MODEM 0
#define PORT_NUM_NMEA 1
#define PORT_NUM_DIAG 2
extern int gser_bind_config(struct usb_configuration *c, u8 port_num);
static int nmea_function_init(struct android_usb_function *f, struct usb_composite_dev *cdev)
{
	pr_info("android_usb: enter %s\n", __func__);
	return 0;
}

static void nmea_function_cleanup(struct android_usb_function *f)
{
	pr_info("android_usb: enter %s\n", __func__);
}

static int nmea_function_bind_config(struct android_usb_function *f, struct usb_configuration *c)
{
	pr_info("android_usb: enter %s\n", __func__);
	return gser_bind_config(c, PORT_NUM_NMEA);
}
static struct android_usb_function nmea_function = {
	.name		= "nmea",
	.init		= nmea_function_init,
	.cleanup	= nmea_function_cleanup,
	.bind_config	= nmea_function_bind_config,
};

static int modem_function_init(struct android_usb_function *f, struct usb_composite_dev *cdev)
{
	pr_info("android_usb: enter %s\n", __func__);
	return 0;
}

static void modem_function_cleanup(struct android_usb_function *f)
{
	pr_info("android_usb: enter %s\n", __func__);
}

extern int modem_bind_config(struct usb_configuration *c, u8 port_num);
static int modem_function_bind_config(struct android_usb_function *f, struct usb_configuration *c)
{
	pr_info("android_usb: enter %s\n", __func__);
	return modem_bind_config(c, PORT_NUM_MODEM);
}
static struct android_usb_function modem_function = {
	.name		= "modem",
	.init		= modem_function_init,
	.cleanup	= modem_function_cleanup,
	.bind_config	= modem_function_bind_config,
};


static int diag_function_init(struct android_usb_function *f, struct usb_composite_dev *cdev)
{
	pr_info("android_usb: enter %s\n", __func__);
	return 0;
}

static void diag_function_cleanup(struct android_usb_function *f)
{
	pr_info("android_usb: enter %s\n", __func__);
}

static int diag_function_bind_config(struct android_usb_function *f, struct usb_configuration *c)
{
	pr_info("android_usb: enter %s\n", __func__);
	return gser_bind_config(c, PORT_NUM_DIAG);
}
static struct android_usb_function diag_function = {
	.name		= "diag",
	.init		= diag_function_init,
	.cleanup	= diag_function_cleanup,
	.bind_config	= diag_function_bind_config,
};
#endif
