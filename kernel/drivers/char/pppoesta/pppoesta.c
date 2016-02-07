#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <linux/slab.h>

#include "pppoesta.h"

static int pppoesta_major = 260;
static int pppoesta_minor = 0;

static struct class* pppoesta_class = NULL;
static struct pppoesta_android_dev* pppoesta_dev = NULL;

static int pppoesta_open(struct inode* inode, struct file* filp);
static int pppoesta_release(struct inode* inode, struct file* filp);
static ssize_t pppoesta_read(struct file* filp, char __user *buf, size_t count, loff_t* f_pos);
static ssize_t pppoesta_write(struct file* filp, const char __user *buf, size_t count, loff_t* f_pos);

static struct file_operations pppoesta_fops = {
    .owner = THIS_MODULE,
    .open = pppoesta_open,
    .release = pppoesta_release,
    .read = pppoesta_read,
    .write = pppoesta_write,
};

static ssize_t pppoesta_val_show(struct device* dev, struct device_attribute* attr,  char* buf);
static ssize_t pppoesta_val_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count);


static DEVICE_ATTR(val, S_IRUGO | S_IWUSR, pppoesta_val_show, pppoesta_val_store);

static int pppoesta_open(struct inode* inode, struct file* filp) {
    struct pppoesta_android_dev* dev;

    printk(KERN_ALERT"pppoesta_open\n");  
    dev = container_of(inode->i_cdev, struct pppoesta_android_dev, dev);
    filp->private_data = dev;

    return 0;
}

static int pppoesta_release(struct inode* inode, struct file* filp) {
    printk(KERN_ALERT"pppoesta_release\n");
    return 0;
}

static ssize_t pppoesta_read(struct file* filp, char __user *buf, size_t count, loff_t* f_pos) {
    ssize_t err = 0;
    struct pppoesta_android_dev* dev = filp->private_data;

    if(down_interruptible(&(dev->sem))) {
        return -ERESTARTSYS;
    }

    if(count < sizeof(dev->val)) {
        goto out;
    }

    if(copy_to_user(buf, dev->val, sizeof(dev->val))) {
        err = -EFAULT;
        goto out;
    }
    printk(KERN_ALERT"pppoesta_read buf: %s.\n", buf);

    err = sizeof(dev->val);

out:
    up(&(dev->sem));
    return err;
}

static ssize_t pppoesta_write(struct file* filp, const char __user *buf, size_t count, loff_t* f_pos) {
    struct pppoesta_android_dev* dev = filp->private_data;
    ssize_t err = 0;

    printk(KERN_ALERT"pppoesta_write buf: %s.\n", buf);
    if(down_interruptible(&(dev->sem))) {
        return -ERESTARTSYS;
    }

    if(count != sizeof(dev->val)) {
        goto out;
    }

    if(copy_from_user(dev->val, buf, count)) {
        err = -EFAULT;
        goto out;
    }

    err = sizeof(dev->val);
  
out:
    up(&(dev->sem));
    return err;
}

static ssize_t __pppoesta_get_val(struct pppoesta_android_dev* hdev, char* buf) {
    char * val;

    printk(KERN_ALERT"__pppoesta_get_val\n");
    if(down_interruptible(&(hdev->sem))) {
        return -ERESTARTSYS;
    }

    val = hdev->val;
    up(&(hdev->sem));
    printk(KERN_ALERT"__pppoesta_get_val  val:%s\n",val);
    return snprintf(buf, PAGE_SIZE, "%s\n", val);
}

static ssize_t __pppoesta_set_val(struct device* dev, struct pppoesta_android_dev* hdev, const char* buf, size_t count) {
    char *uevent[2];
    char msg[MAX_MSG_LENGTH];
    printk(KERN_ALERT"__pppoesta_set_val\n");
    if(down_interruptible(&(hdev->sem))) {
        return -ERESTARTSYS;
    }
    strncpy(hdev->val, buf, VAL_LENGTH - 1);
    hdev->val[VAL_LENGTH - 1] = '\0';
    printk(KERN_ALERT"__pppoesta_set_val snprintf  val:%s\n",hdev->val);
    snprintf(msg, sizeof(msg), "PPPOESTA=%s",hdev->val);
    uevent[0] = msg;
    uevent[1] = NULL;
    printk(KERN_ALERT"__pppoesta_set_val kobject_uevent_env\n");
    kobject_uevent_env(&(dev->kobj), KOBJ_CHANGE, (char**)&uevent);
    up(&(hdev->sem));
    return count;
}

static ssize_t pppoesta_val_show(struct device* dev, struct device_attribute* attr, char* buf) {
    struct pppoesta_android_dev* hdev = (struct pppoesta_android_dev*)dev_get_drvdata(dev);
    printk(KERN_ALERT"pppoesta_val_show\n");
    return __pppoesta_get_val(hdev, buf);
}

static ssize_t pppoesta_val_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count) {
    struct pppoesta_android_dev* hdev = (struct pppoesta_android_dev*)dev_get_drvdata(dev);
      printk(KERN_ALERT"pppoesta_val_store\n");
    return __pppoesta_set_val(dev, hdev, buf, count);
}

static int  __pppoesta_setup_dev(struct pppoesta_android_dev* dev) {
    int err;
    dev_t devno = MKDEV(pppoesta_major, pppoesta_minor);
    printk(KERN_ALERT"__pppoesta_setup_dev\n");
    memset(dev, 0, sizeof(struct pppoesta_android_dev));

    cdev_init(&(dev->dev), &pppoesta_fops);
    dev->dev.owner = THIS_MODULE;
    dev->dev.ops = &pppoesta_fops;

    err = cdev_add(&(dev->dev),devno, 1);
    if(err) {
        return err;
    }

    sema_init(&(dev->sem),1);
    memset(dev->val, 0, VAL_LENGTH);

    return 0;
}

static int __init pppoesta_init(void){
    int err = -1;
    dev_t dev = 0;
    struct device* temp = NULL;

    printk(KERN_ALERT"Initializing pppoesta device.\n");

    err = alloc_chrdev_region(&dev, 0, 1, PPPOESTA_DEVICE_NODE_NAME);
    if(err < 0) {
        printk(KERN_ALERT"Failed to alloc char dev region.\n");
        goto fail;
    }

    pppoesta_major = MAJOR(dev);
    pppoesta_minor = MINOR(dev);

    pppoesta_dev = kmalloc(sizeof(struct pppoesta_android_dev), GFP_KERNEL);
    if(!pppoesta_dev) {
        err = -ENOMEM;
        printk(KERN_ALERT"Failed to alloc pppoesta_dev.\n");
        goto unregister;
    }

    err = __pppoesta_setup_dev(pppoesta_dev);
    if(err) {
        printk(KERN_ALERT"Failed to setup dev: %d.\n", err);
        goto cleanup;
    }

    pppoesta_class = class_create(THIS_MODULE, PPPOESTA_DEVICE_CLASS_NAME);
    if(IS_ERR(pppoesta_class)) {
        err = PTR_ERR(pppoesta_class);
        printk(KERN_ALERT"Failed to create pppoesta class.\n");
        goto destroy_cdev;
    }

    temp = device_create(pppoesta_class, NULL, dev, "%s", PPPOESTA_DEVICE_FILE_NAME);
    if(IS_ERR(temp)) {
        err = PTR_ERR(temp);
        printk(KERN_ALERT"Failed to create pppoesta device.");
        goto destroy_class;
    }

    err = device_create_file(temp, &dev_attr_val);
    if(err < 0) {
        printk(KERN_ALERT"Failed to create attribute val.");
        goto destroy_device;
    }

    dev_set_drvdata(temp, pppoesta_dev);


    printk(KERN_ALERT"Succedded to initialize pppoesta device.\n");
    return 0;

destroy_device:
    device_destroy(pppoesta_class, dev);

destroy_class:
    class_destroy(pppoesta_class);

destroy_cdev:
    cdev_del(&(pppoesta_dev->dev));

cleanup:
    kfree(pppoesta_dev);

unregister:
    unregister_chrdev_region(MKDEV(pppoesta_major, pppoesta_minor), 1);

fail:
    return err;
}

static void __exit pppoesta_exit(void) {
    dev_t devno = MKDEV(pppoesta_major, pppoesta_minor);

    printk(KERN_ALERT"Destroy pppoesta device.\n");

    if(pppoesta_class) {
        device_destroy(pppoesta_class, MKDEV(pppoesta_major, pppoesta_minor));
        class_destroy(pppoesta_class);
    }

    if(pppoesta_dev) {
        cdev_del(&(pppoesta_dev->dev));
        kfree(pppoesta_dev);
    }

    unregister_chrdev_region(devno, 1);
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("pppoe status Driver");

module_init(pppoesta_init);
module_exit(pppoesta_exit);

