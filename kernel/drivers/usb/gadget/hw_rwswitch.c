/*
 * huawei rework switch code.
 *
 */
#include <linux/workqueue.h>
#include <linux/mutex.h>

/* #define DEBUG */
/* #define VERBOSE_DEBUG */


/* 0: no usb port switch request has been sent
 * 1: one usb port switch request has already been sent at least
 */
static int switch_request = 0;
static int port_mode = 0;
static bool switch_done = false;


static int usb_port_switch(int usb_switch)
{
	int ret = 0;
	char event_buf[32];
	char *envp[2] = {event_buf, NULL};

	snprintf(event_buf, sizeof(event_buf),"USB_PORT_SWITCH=%d", usb_switch);
	if (_android_dev == NULL) {
		pr_info("%s: _android_dev is NULL\n", __func__);
		return ret;
	}
	if (_android_dev->dev == NULL) {
		pr_info("%s: _android_dev->dev is NULL\n", __func__);
		return ret;
	}
	if (&_android_dev->dev->kobj == NULL) {
		pr_info("%s: &_android_dev->dev->kobj is NULL\n", __func__);
		return ret;
	}
	ret= kobject_uevent_env(&_android_dev->dev->kobj, KOBJ_CHANGE, envp);
	if (ret < 0) {
		pr_info("%s: uevent USB_PORT_SWITCH=%d sending OK\n", __func__, usb_switch);
	} else {
		pr_err("%s: uevent USB_PORT_SWITCH=%d sending failed %d\n", __func__, usb_switch, ret);
	}

	return ret;
}
static void usb_port_switch_wq(struct work_struct *data)
{
	usb_port_switch(switch_request);
}
DECLARE_WORK(usb_port_switch_work, usb_port_switch_wq);

/*
 *  * usb_port_switch_request: submit usb switch request by sending uevent
 *  * Return value: int
 *  * Side effect : none
 *  */
int usb_port_switch_request(int usb_switch_index)
{
	if (0 == usb_switch_index) {    //No switch
		pr_info("%s: uevent USB_PORT_SWITCH=%d not send\n", __func__, usb_switch_index);
		return 0;
	}

	switch_request = usb_switch_index;

	if (in_interrupt())
		schedule_work(&usb_port_switch_work);
	else
		return usb_port_switch(switch_request);

	return 0;
}
EXPORT_SYMBOL(usb_port_switch_request);

static ssize_t switch_request_show(struct device *pdev, struct device_attribute *attr,
			   char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%d\n", switch_done ? switch_request : 0);
}

static ssize_t switch_request_store(struct device *pdev, struct device_attribute *attr,
			    const char *buff, size_t size)
{
	char event_buf[32];
	char *envp[2] = {event_buf, NULL};

	snprintf(event_buf, sizeof(event_buf),"USB_PORT_SWITCH_DONE=%d", switch_request);
	if (_android_dev == NULL) {
		pr_info("%s: _android_dev is NULL\n", __func__);
		return 0;
	}
	if (_android_dev->dev == NULL) {
		pr_info("%s: _android_dev->dev is NULL\n", __func__);
		return 0;
	}
	if (&_android_dev->dev->kobj == NULL) {
		pr_info("%s: &_android_dev->dev->kobj is NULL\n", __func__);
		return 0;
	}

	kobject_uevent_env(&_android_dev->dev->kobj, KOBJ_CHANGE, envp);

	switch_done = true;

	return size;
}

static ssize_t port_mode_show(struct device *pdev, struct device_attribute *attr,
			   char *buf)
{
    return snprintf(buf, PAGE_SIZE, "%d\n", port_mode);
}
static ssize_t port_mode_store(struct device *dev, struct device_attribute *attr,
		const char *buf, size_t size)
{
	pr_info("%s: port_mode buf=\"%s\", size=%lu\n", __func__, buf, size);

	if (sscanf(buf, "%2d", &port_mode) == 1) {
		return strlen(buf);
	}

	return -1;
}
int usb_port_mode_get(void)
{
	return port_mode;
}

static ssize_t switch_index_show(struct device *pdev, struct device_attribute *attr,
			   char *buf)
{
    return snprintf(buf, PAGE_SIZE, "%d\n", switch_request);
}
static DEVICE_ATTR(switch_request, S_IRUGO | S_IWUSR, switch_request_show, switch_request_store);
static DEVICE_ATTR(switch_index, S_IRUGO, switch_index_show, NULL);
static DEVICE_ATTR(port_mode, 0660, port_mode_show, port_mode_store);

