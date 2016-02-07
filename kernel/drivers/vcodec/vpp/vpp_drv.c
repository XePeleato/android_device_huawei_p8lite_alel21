
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/syscalls.h>
#include <asm/irq.h>
#include <media/v4l2-common.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/timex.h>
#include <linux/of_irq.h>
#include <linux/regulator/driver.h>
#include <linux/platform_device.h>
#include <linux/atomic.h>
#include <asm/io.h>
#include <linux/clk.h>

#include "vpp_drv.h"

#define LOG_TAG "VPP_DRV"
#include "vpp_log.h"

/* module description */
MODULE_LICENSE("Proprietary");
MODULE_AUTHOR("j00140427");
MODULE_DESCRIPTION("vpp driver");

#define VPP_PLATFORM_DEVICE_NAME "hisilicon,k3vpp"
#define VPP_DEV_NAME "k3vpp"
#define VPP_LDO_NAME "ldo_vpp"

static struct class *s_vpp_class = NULL;
static int s_vpp_major = -1;
static struct file_operations vpp_file_ops;

static struct cdev *s_vpp_device = NULL;

static int s_vpp_irq   = 0;
static wait_queue_head_t s_wait_queue;
static bool s_interrupt_cond = false;

unsigned int g_mmu_ch;
//VPP主设备open次数
static unsigned int s_vpp_count = 0;
typedef struct vpp_drv_context_t {
    struct fasync_struct *async_queue;
} vpp_drv_context_t;

static struct clk *s_vpp_clk;
static struct regulator_bulk_data vpp_regulator = {0};
static void vpp_clk_disable(struct clk *clk);
static int vpp_clk_enable(struct clk *clk);
static void vpp_clk_put(struct clk *clk);
/*the func regist to irq*/
static irqreturn_t vpp_isr ( int irq, void *dev_id )
{
    if (hal_get_int_sta(HAL_INTMSK_VTEINT))
    {
        logd("============== the irq of vpp is my care\n");
        s_interrupt_cond = true;
        wake_up_interruptible(&s_wait_queue);
        hal_clear_int_sta(HAL_INTMSK_VTEINT);
    }
    else
    {
        logd("============== the irq is not my care\n");
    }

    return IRQ_HANDLED;
}

static int vpp_setup_cdev(void)
{
    struct device *dev = NULL;
    unsigned int minor = 0;
    struct cdev  *device = NULL;
    int retval = -1;;
    logi("in vpp_setup_cdev\n");
    retval = -ENOMEM;
    device = kzalloc(sizeof(*device), GFP_KERNEL);

    if (device == NULL)
    {
        loge(KERN_ERR " Unable to allocate vpp device\n");
        return retval;
    }


    cdev_init(device, &vpp_file_ops);
    device->owner = THIS_MODULE;
    device->ops = &vpp_file_ops;

    retval = cdev_add(device, MKDEV(s_vpp_major, minor), 1);
    if (retval)
    {
        loge(KERN_ERR " chardev registration failed\n");
        kfree(device);
        return retval;
    }

    dev = device_create(s_vpp_class, 0, MKDEV(s_vpp_major, minor), NULL, VPP_DEV_NAME"%u", minor);
    if (NULL == dev)
    {
        loge(KERN_ERR " Can't create device\n");
    }
    s_vpp_device = device;
    return retval;
}

static int vpp_probe(struct platform_device *pdev)
{
    struct device *device       = &pdev->dev;
    struct device_node *node    = device->of_node;
    int retval;
    dev_t dev;
    memset(&dev, 0x00, sizeof(dev));
    logi("in vpp_probe\n");
    if (!node) {
        loge(KERN_ERR "[VPPDRV] NOT FOUND device node %s!\n", VPP_PLATFORM_DEVICE_NAME);
        goto ERROR_PROVE_DEVICE;
    }

    vpp_regulator.supply = VPP_LDO_NAME;
    retval = regulator_bulk_get(device, 1, &vpp_regulator);
    if (retval){
        loge(KERN_ERR "[VPPDRV] Couldn't get regulators err=%d\n", retval);
        goto ERROR_PROVE_DEVICE;
    }
    s_vpp_clk = of_clk_get(node,0);
    if (IS_ERR(s_vpp_clk)) {
        loge(KERN_ERR "[VPPDRV] FAIL to get clock controller\n");
        goto ERROR_PROVE_DEVICE;
    }
    else {
        logi("vpp get clock controller,s_vpp_clk=%p\n", s_vpp_clk);
    }
    s_vpp_class = class_create(THIS_MODULE, "vpp");
    if (NULL == s_vpp_class)
    {
        retval =  -ENOMEM;
        loge(KERN_ERR "[VPPDRV] alloc video device failed retval = %d\n",retval);
        goto ERROR_PROVE_DEVICE;
    }

    retval = alloc_chrdev_region(&dev, 0, 1, "vpp");
    if (retval)
    {
        loge(KERN_ERR "[VPPDRV] Can't register character device retval = %d\n",retval);
        goto ERROR_PROVE_DEVICE;
    }

    s_vpp_major = MAJOR(dev);
    if (s_vpp_major < 0){
        loge(KERN_ERR "[VPPDRV] Can't get vpp device major\n");
        goto ERROR_PROVE_DEVICE;
    }
    s_vpp_irq = irq_of_parse_and_map(node, 0);
    logi(" vpp irq = %d\n",s_vpp_irq);
    retval = request_irq(s_vpp_irq, vpp_isr, 0, "VPP", NULL);

    if(retval)
    {
        loge("fail to request vpp irq, ret = 0x%x\n",retval);
        goto ERROR_PROVE_DEVICE;
    }
    retval = vpp_setup_cdev( );
    if (retval)
    {
        loge(KERN_ERR "[VPPDRV] Can't register char driver retval = %d\n",retval);
        goto ERROR_PROVE_DEVICE;
    }
    retval = hal_init(node);
    if (retval){
        loge(KERN_ERR "[VPPDRV] hal init is fail\n");
        goto ERROR_PROVE_DEVICE;
    }
    return 0;
ERROR_PROVE_DEVICE:
    if (s_vpp_irq){
        free_irq(s_vpp_irq, (void *)0);
        s_vpp_irq = 0;
    }
    if(s_vpp_major){
       unregister_chrdev(s_vpp_major, VPP_DEV_NAME);
       s_vpp_major = 0;
    }
    if(s_vpp_class){
        class_destroy(s_vpp_class);
        s_vpp_class = NULL;
    }

    if (!IS_ERR(s_vpp_clk)){
        vpp_clk_put(s_vpp_clk);
        //goto err;
    }
    regulator_put(vpp_regulator.consumer);
    memset(&vpp_regulator, 0, sizeof(vpp_regulator));

    return -1;
}
static int vpp_remove(struct platform_device *pdev)
{
    if (s_vpp_major > 0)
    {
        unregister_chrdev(s_vpp_major, VPP_DEV_NAME);
        s_vpp_major = 0;
    }
    if (s_vpp_irq){
        free_irq(s_vpp_irq, 0);
        s_vpp_irq = 0;
    }
    if(!IS_ERR(s_vpp_clk)){
        vpp_clk_put(s_vpp_clk);
    }
    regulator_bulk_disable(1, &vpp_regulator);
    memset(&vpp_regulator, 0, sizeof(vpp_regulator));
    hal_deinit();
    return 0;
}
static int vpp_suspend(struct platform_device *pdev,pm_message_t state){
        printk("%s+.\n",__func__);
#if 0
        vpp_clk_disable(s_vpp_clk);
        regulator_bulk_disable(1, &vpp_regulator);
#endif
        printk("%s-.\n",__func__);
        return 0;
}
static int vpp_resume(struct platform_device *pdev){
        //int err = 0;
        printk("%s+.\n",__func__);
#if 0
        err = regulator_bulk_enable(1, &vpp_regulator);
        if (err){
            loge(KERN_ERR "[VPPDRV] failed to enable vpp regulators %d\n", err);
            return err;
        }

        err = vpp_clk_enable(s_vpp_clk);
        if (err){
            loge(KERN_ERR "[VPPDRV] failed to enable vpp clk %d\n", err);
            regulator_bulk_disable(1, &vpp_regulator);
            return err;
        }
#endif
        printk("%s-.\n",__func__);
        return 0;
}
static const struct of_device_id vpp_project_match_table[] = {
    {
        .compatible = VPP_PLATFORM_DEVICE_NAME,
        .data = NULL,
    },
    {
    },
};
static struct platform_driver vpp_driver = {
    .driver  = {
    .name    = VPP_PLATFORM_DEVICE_NAME,
    .owner   = THIS_MODULE,
    .of_match_table = of_match_ptr(vpp_project_match_table),
           },
    .probe   = vpp_probe,
    .remove  = vpp_remove,
    .suspend = vpp_suspend,
    .resume  = vpp_resume,
};
static int __init vpp_init(void)
{
    int ret = 0;
    logi("vpp init\n");
    init_waitqueue_head(&s_wait_queue);
    ret = platform_driver_register(&vpp_driver);
    printk("end vpp_init result=0x%x\n", ret);
    return ret;
}

void __exit vpp_deinit(void)
{
    struct cdev *pDev = s_vpp_device;
    unsigned int minor = 0;
    logi("in vpp_deinit\n");

    if (s_vpp_device)
    {
        minor = MINOR(s_vpp_device->dev);
        device_destroy(s_vpp_class, MKDEV(s_vpp_major, minor));
        cdev_del(pDev);
        kfree(pDev);
        s_vpp_device = NULL;
    }
    if(s_vpp_major){
    unregister_chrdev_region(MKDEV(s_vpp_major, 0), 1);
        s_vpp_major = 0;
    }
    if(s_vpp_class){
    class_destroy(s_vpp_class);

        s_vpp_class = NULL;
    }

    platform_driver_unregister(&vpp_driver);
    return;
}

static int vpp_open(struct inode *inode, struct file *file)
{
    int ret = -EFAULT;
    int i = 0;
    logi("in vpp_open\n");

    if ( s_vpp_count > 0 )
    {
        logi("don't need to open device, the open count is %d\n", s_vpp_count);
        s_vpp_count++;
        return 0;
    }

    ret = regulator_bulk_enable(1, &vpp_regulator);
    if (ret){
        printk("failed to enable vpp regulators ret=%#x\n", ret);
        return -1;
    }

        ret = vpp_clk_enable(s_vpp_clk);
        if (ret){
            loge("failed to enable vpp clock ret = %#x\n",ret);
        regulator_bulk_disable(1, &vpp_regulator);
        return -1;
        }
        else{
            logi("vpp clock is enable\n");
    }

    ret = inter_init();
    if(K3_SUCCESS != ret)
    {
        loge("vpp init failed ret = %#x\n",ret);
        goto ERROR_OPEN_DEVICE;
    }

    inter_open(HAL_LAYER_VIDEO1);
    for (i=0;i<11;i++)
    {
        hal_enb_mmu_ch_inte(i,1);
    }

    hal_set_int_enable(HAL_INTMSK_VTEINT);
    s_vpp_count++;

    return 0;
ERROR_OPEN_DEVICE:
    vpp_clk_disable(s_vpp_clk);
    regulator_bulk_disable(1, &vpp_regulator);
    return -1;
}

static int vpp_close(struct inode *inode, struct file *file)
{
    logi("in vpp_close\n");

    s_vpp_count--;
    logi("in vpp_close s_vpp_count = %d\n",s_vpp_count);
    if (s_vpp_count > 0)//TODO: < 0, 试一下
    {
        logi("don't need to close device, the open count is %d\n", s_vpp_count);
        return 0;
    }

    inter_close(HAL_LAYER_VIDEO1);

    inter_deinit();

        vpp_clk_disable(s_vpp_clk);

    /*disable vdec regulator*/
    regulator_bulk_disable(1, &vpp_regulator);

    return 0;
}

static long vpp_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    int err = 0;
    logd("in vpp_ioctl\n");
    /*
     * extract the type and number bitfields, and don't decode
     * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
     */
    if(_IOC_TYPE(cmd) != VPP_IOC_MAGIC)
    {
        loge("vpp magic is wrong\n");
        return -ENOTTY;
    }

    if(_IOC_NR(cmd) > VPP_IOC_MAXNR)
    {
        loge("vpp NR is wrong\n");
        return -ENOTTY;
    }

    if(_IOC_DIR(cmd) & _IOC_READ)
    {
        err = !access_ok(VERIFY_WRITE, (void *) arg, _IOC_SIZE(cmd));
    }
    else if(_IOC_DIR(cmd) & _IOC_WRITE)
    {
        err = !access_ok(VERIFY_READ, (void *) arg, _IOC_SIZE(cmd));
    }

    if(err)
    {
        loge("vpp err: %d\n",err);
        return -EFAULT;
    }

    switch (cmd)
    {
        case VPP_STARTCONFIG:
        {
            VPP_CONFIG_S vpp_config;

            logd("VPP_STARTCONFIG\n");

            err = __copy_from_user(&vpp_config, (const void *) arg, sizeof(VPP_CONFIG_S));
            if (err)
            {
                loge("__copy_from_user failed\n");
                err =  -EFAULT;
                break;
            }

            s_interrupt_cond = false;

            err = inter_start( &vpp_config );
            if( K3_SUCCESS == err )
            {   //fixme:time
                int ret = wait_event_interruptible_timeout(s_wait_queue, s_interrupt_cond, (HZ*20));

                s_interrupt_cond = false;

                if( ret > 0 )
                {
                    err = 0;
                    //log_reg_value();
                    logd("wait event success\n");
                }
                else
                {
                    loge("wait event time out\n");
                    log_reg_value();
                    err = -ETIME;
                }
            }
            else
            {
                err = -EFAULT;
                loge("set vpp config failed\n");
            }
            return err;
            break;
        }

      case VPP_GETADDR:
        {
            loge("VPP get address\n");

             __put_user(0x32000000, (unsigned long *)arg);
            break;
        }
        default:
        {
            loge("unsurport cmd cmd = %d\n",cmd);
            err = -ENOIOCTLCMD;
            break;
        }
    }

    return err;
}
#ifdef VPP_MMAP
//only for test
static int vpp_mmap(struct file *file, struct vm_area_struct *vma)
{
    unsigned long start = 0;
    unsigned long size = 0;

    logi("in vpp_mmap.");

    if (NULL == vma)
    {
        loge("can not get vm_area_struct!");
        return -EBADF;
    }

    start = vma->vm_start;
    size = vma->vm_end - vma->vm_start;

    /* make buffers write-thru cacheable */
    vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot); //update to 2.6.32
    if (remap_pfn_range(vma, start, vma->vm_pgoff, size, vma->vm_page_prot))
    {
        loge("remap_pfn_range error!");
        return -ENOBUFS;
    }

    logi("out vpp_mmap.");
    return 0;
}
#endif
void vpp_clk_put(struct clk *clk)
{
    if (clk)
        clk_put(clk);
}
int vpp_clk_enable(struct clk *clk)
{
    if (IS_ERR(clk)) {
        loge(KERN_ERR "[VPPDRV] vpp clock is null\n");
        return 0;
    }
    return clk_prepare_enable(clk);
}
void vpp_clk_disable(struct clk *clk)
{
    if (IS_ERR(clk)) {
        loge(KERN_ERR "[VPPDRV] vpp clock is null\n");
        return;
    }
    clk_disable_unprepare(clk);
}

static struct file_operations vpp_file_ops =
{
    .open = vpp_open,
    .release = vpp_close,
    .compat_ioctl = vpp_ioctl,
#ifdef VPP_MMAP
    .mmap = vpp_mmap,
#endif
};

module_init(vpp_init);
module_exit(vpp_deinit);
MODULE_LICENSE("GPL");

