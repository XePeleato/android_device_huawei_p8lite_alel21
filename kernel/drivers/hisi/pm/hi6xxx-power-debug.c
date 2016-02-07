#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/debugfs.h>
#include <asm/uaccess.h>
#include <linux/io.h>
#include <linux/hisi/pm/pwrctrl_multi_memcfg.h>
#include <linux/hisi/pm/pwrctrl_multi_sleep.h>
#include "../mailbox/hi6xxx_mailbox/drv_mailbox_test_linux.h"

#define BUF_SIZE    (400)
#define MCU_STATE   (0)

static void __iomem *g_mculog_addr;

static struct dentry *cntdump_dbgfs;
static struct dentry *wkupccpu_dbgfs;


static ssize_t cntdump_dbgfs_read(struct file *fp, char __user *ubuf,
					size_t cnt, loff_t *ppos)
{
    int ret = 0;
    int pos = 0;
    unsigned int cntdump_buf[8];
    char *cntdump_tmp = NULL;

    PWC_MCU_LOG_STRU *p_mcu_exc_log;
	
    void __iomem *mcu_log_addr;

    mcu_log_addr = ioremap(MEMORY_AXI_MCU_LOG_ADDR, MEMORY_AXI_MCU_LOG_SIZE);
    if(!mcu_log_addr)
	{
		printk(KERN_ERR "pwrctrl_debug mcu_log_addr ioremap error.\n");
		return -ENOMEM;
	}

    g_mculog_addr = ioremap(readl(mcu_log_addr), sizeof(PWC_MCU_LOG_STRU));
    if(!g_mculog_addr)
	{
		printk(KERN_ERR "pwrctrl_debug g_mculog_addr ioremap error.\n");
        iounmap(mcu_log_addr);
		return -ENOMEM;
	}

    p_mcu_exc_log = (PWC_MCU_LOG_STRU *)g_mculog_addr;
	
    cntdump_tmp = kzalloc(BUF_SIZE,GFP_KERNEL);
	if(!cntdump_tmp){
        iounmap(g_mculog_addr);
        iounmap(mcu_log_addr);
        return -ENOMEM;
    }

    cntdump_buf[0] = p_mcu_exc_log->ccpu.pdStat.sleepCount;
    cntdump_buf[1] = p_mcu_exc_log->ccpu.puStat.wakeCount;
    cntdump_buf[2] = p_mcu_exc_log->mcu.info[MCU_STATE].stat.sleepCount;
    cntdump_buf[3] = p_mcu_exc_log->mcu.info[MCU_STATE].stat.wakeCount;
	cntdump_buf[4] = p_mcu_exc_log->acpu.pdStat.sleepCount;
    cntdump_buf[5] = p_mcu_exc_log->acpu.puStat.wakeCount;
    cntdump_buf[6] = p_mcu_exc_log->hifi.pdStat.sleepCount;
    cntdump_buf[7] = p_mcu_exc_log->hifi.puStat.wakeCount;

    pos += snprintf(cntdump_tmp + pos,BUF_SIZE - pos,"ccpu sleep: %d; \t",cntdump_buf[0]);
    pos += snprintf(cntdump_tmp + pos,BUF_SIZE - pos,"ccpu wake: %d.\t\n",cntdump_buf[1]);
    pos += snprintf(cntdump_tmp + pos,BUF_SIZE - pos,"mcu  sleep:%d;\t",cntdump_buf[2]);
    pos += snprintf(cntdump_tmp + pos,BUF_SIZE - pos,"mcu  wake:%d\t\n",cntdump_buf[3]);
	pos += snprintf(cntdump_tmp + pos,BUF_SIZE - pos,"acpu  sleep:%d;\t",cntdump_buf[4]);
    pos += snprintf(cntdump_tmp + pos,BUF_SIZE - pos,"acpu  wake:%d\t\n",cntdump_buf[5]);
	pos += snprintf(cntdump_tmp + pos,BUF_SIZE - pos,"hifi  sleep:%d;\t",cntdump_buf[6]);
    pos += snprintf(cntdump_tmp + pos,BUF_SIZE - pos,"hifi  wake:%d\t\n",cntdump_buf[7]);

    /*transfer sleep counter log message to user buf*/
	ret = simple_read_from_buffer(ubuf, cnt, ppos, cntdump_tmp, pos);
    if(0 == ret){
        pr_info("pwrctrl_debug cntdump_dbgfs_read data transfer success\n");
    }

    iounmap(g_mculog_addr);
    iounmap(mcu_log_addr);
    kfree(cntdump_tmp);
	return ret;

}

static ssize_t wkupccpu_dbgfs_write(struct file *filp, const char __user *ubuf, size_t cnt, loff_t *ppos)
{
	char buf[128];
	char *cmd = NULL;
    unsigned int DstId = 1;
    unsigned int MsgLen = 1;
    unsigned int MsgNum = 1;
    unsigned int DelayNum = 0;
    
	if (copy_from_user(buf, ubuf, cnt - 1)) {
		pr_err("pwrctrl_debug [wkupccpu DEBUGFS] can not copy from user\n");
		return -EINVAL;
	}

	buf[cnt - 1] = '\0';
	cmd = buf;

	if (!strncmp("wpccpu", cmd, strlen("wpccpu"))) {
		test_mailbox_msg_single_test(DstId,MsgLen,MsgNum,DelayNum);
	}
    else {
		pr_err("pwrctrl_debug [wkupccpu DEBUGFS] cmd error\n");
		cnt = -EINVAL;
    }

	return cnt;

}


static struct file_operations cntdump_fops = {
	.owner = THIS_MODULE,
	.read = cntdump_dbgfs_read,
};

static struct file_operations wkupccpu_fops = {
	.owner = THIS_MODULE,
	.write = wkupccpu_dbgfs_write,
};
static int __init pwrctrl_debug_init(void)
{

    struct dentry *cntdump_file;
    struct dentry *wkupccpu_file;
    
    pr_info("-------------enter pwrctrl_debug_init---------\n");

    cntdump_dbgfs = debugfs_create_dir("cntdump_dbgfs", NULL);
	if (!cntdump_dbgfs)
		return -ENOENT;

	cntdump_file = debugfs_create_file("cntdump", 0644, cntdump_dbgfs, NULL, &cntdump_fops);
	if (!cntdump_file)
		goto Fail1;      

    wkupccpu_dbgfs = debugfs_create_dir("wkupccpu_dbgfs", NULL);
	if (!wkupccpu_dbgfs)
		goto Fail1;

	wkupccpu_file = debugfs_create_file("wkupccpu", 0644, wkupccpu_dbgfs, NULL, &wkupccpu_fops);
	if (!wkupccpu_file)
		goto Fail0; 
    
    return 0;
    
Fail0:
	debugfs_remove_recursive(wkupccpu_dbgfs);
	wkupccpu_dbgfs = NULL;
    printk(KERN_ERR "create wkupccpu file fail\n");
    
Fail1:
	debugfs_remove_recursive(cntdump_dbgfs);
	cntdump_dbgfs = NULL;
    printk(KERN_ERR "create countdump file fail\n");
	return -ENOENT;
}

static void __exit pwrctrl_debug_exit(void)
{
    printk(KERN_INFO "freqdump_debugfs_exit\n");

	debugfs_remove_recursive(cntdump_dbgfs);
    debugfs_remove_recursive(wkupccpu_dbgfs);
        
    return;
}

module_init(pwrctrl_debug_init);
module_exit(pwrctrl_debug_exit);

MODULE_LICENSE("GPL");

