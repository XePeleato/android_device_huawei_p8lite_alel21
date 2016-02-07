#include <linux/err.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/hisi/reset.h>
#include <hi6xxx_loadmodem.h>

DECLARE_COMPLETION_ONSTACK(modem_depend_complete);

extern int usb_cshell_init_work(void);
extern int VOS_ModuleInit(void);
extern int NV_Init(void);
#ifdef BSP_C_HIFI_RESET_ALONE_FEATURE
extern int reset_sub_mgr_init(void);
#endif
extern void cshell_icc_open(void);
extern int BSP_ICC_Init(void);
extern int rfile_init_thread(void);
extern int IPF_Init(void);
extern int BSP_MEM_Init(void);
extern void BSP_SYNC_init(void);
extern int ccore_reset_module_init(void);

static struct workqueue_struct *modem_init_workqueue;

static void modem_init_work_func(struct work_struct *work)
{
    int ret;

    BSP_SYNC_init();

    ret = loadmodem_init();
    if (ret) {
        printk(KERN_ERR "%s loadmodem_init init fail ",__func__);
    }

    ret = BSP_MEM_Init();
    if (ret) {
        printk(KERN_ERR "%s BSP_MEM_Init init fail ",__func__);
    }

    ret = BSP_ICC_Init();
    if (ret) {
        printk(KERN_ERR "%s BSP_ICC_Init init fail ",__func__);
    }

    ret = IPF_Init();
    if (ret) {
        printk(KERN_ERR "%s IPF INIT FAIL\n",__func__);
    }

    cshell_icc_open();

#ifdef BSP_C_HIFI_RESET_ALONE_FEATURE
    ret = reset_sub_mgr_init();
    if (ret) {
        printk(KERN_ERR "%s reset_sub_mgr_init init fail ",__func__);
    }

    ccore_reset_module_init();
#endif

    ret = usb_cshell_init_work();
    if (ret) {
        printk(KERN_ERR "%s cshell init fail ",__func__);
    }

    wait_for_completion(&modem_depend_complete);

    ret = rfile_init_thread();
    if (ret) {
        printk(KERN_ERR "%s rfile init fail ",__func__);
    }
    printk(KERN_ERR "%s is or not come in NV init!\n",__func__);
    ret = NV_Init();
    if (ret) {
        printk(KERN_ERR "%s NV init fail ",__func__);
    }

#ifdef BSP_COMPILE_ALLY
    printk(KERN_ERR "%s is or not come in VOS_ModuleInit!\n",__func__);
    ret = VOS_ModuleInit();
    if (ret) {
        printk(KERN_ERR "%s vos module init fail ",__func__);
    }
#endif
    return;
}

static ssize_t modem_work_depend_file_write(struct file *file, const char __user * buffer,
                size_t count, loff_t * ppos)
{
    char buf[2] = {0};

    if (count > sizeof(buf) - 1)
        return -EINVAL;
    if (copy_from_user(buf, buffer, count))
        return -EFAULT;

    if ('1' == buf[0]) {
    complete(&modem_depend_complete);
    printk(KERN_INFO "modem depend write ok %s\n", __func__);
    } else {
        printk(KERN_ERR "the value modem depend write is illegal %s\n", __func__);
    }
    return 0;
}

static const struct file_operations modem_work_depend_file_ops = {
    .owner = THIS_MODULE,
    .write = modem_work_depend_file_write,
};

extern int check_acore_only(void);

static int __init modem_init(void)
{
    static struct work_struct modem_init_work;
    struct proc_dir_entry *entry;

    /*added by zhuzheng for v8r2*/
    if (1 == check_acore_only())
        return 0;

    if (mmc_read_done()){
        printk(KERN_ERR "%s load modem mmc_read_done failed\n",__func__);
        return -1;
    }

    modem_init_workqueue = alloc_ordered_workqueue("kmodeminitqd", 0);

    if (!modem_init_workqueue) {
        printk(KERN_ERR "%s start modem init  qd failed\n",__func__);
        return -1;
    }

    entry = proc_create("modem_depend", 0660, NULL, &modem_work_depend_file_ops);
    if (!entry) {
        printk(KERN_ERR "modem_depend: failed to create proc entry\n");
        goto destroy_workqueue;
    }

    INIT_WORK(&modem_init_work, modem_init_work_func);

    queue_work(modem_init_workqueue, &modem_init_work);

    goto out;

    destroy_workqueue:
        destroy_workqueue(modem_init_workqueue);
    out:

    return 0;
}

subsys_initcall(modem_init);

