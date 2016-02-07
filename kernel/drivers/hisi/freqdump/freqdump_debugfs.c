#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/debugfs.h>
#include <asm/uaccess.h>
#include <linux/io.h>
#include <linux/regulator/consumer.h>
#include <linux/hisi/pm/pwrctrl_multi_memcfg.h>
#include <linux/mfd/hisi_hi6xxx_pmic.h>
#include <linux/seq_file.h>

#define BUFSZ (200)
#define PMU_VERSION_MIN (0x30)
#define PMU_HI6553_V110 (0x11)
#define SOC_SMART_BUCK01_CTRL3_ADDR(base)             ((base) + (0xDA))
#define SOC_SMART_VSET_BUCK1_ADJ_ADDR(base)       ((base) + (0x06C))
static struct dentry *freqdump_debugfs;
static u32 acpu_volt_reg = 0;
static u32 acpu_volt = 0;
static u32 pmu_version = 0;
static struct freqdump *freqdump_tmp = NULL;
static char freqdump_buf[BUFSZ] = {0}; 
static void __iomem *freqdump_virt_addr = NULL;
struct freqdump
{
    unsigned int freq_acpu_freq_last;
    unsigned int freq_acpu_freq_cur;

    unsigned int freq_acpu_load;
    unsigned int freq_acpu_core_online;
    unsigned int freq_every_acpu_load[4];

    unsigned int freq_ddr_freq_last;
    unsigned int freq_ddr_freq_cur;
    unsigned int freq_ddr_data_load;
    unsigned int freq_ddr_cmd_load;

    unsigned int freq_ccpu_freq_last;
    unsigned int freq_ccpu_freq_cur;
    unsigned int freq_ccpu_load;

};
static ssize_t freqdump_dbgfs_read(struct seq_file *s, void *p)
{
    int ret = 0;
    int pos = 0;
    
    /* get acpu volt according to PMU_version  and transfer to user space*/
    if(PMU_VERSION_MIN <= pmu_version){
        /*hi6552 pmu chipid*/
        acpu_volt_reg = (unsigned int)hi6xxx_pmic_reg_read(SOC_SMART_VSET_BUCK1_ADJ_ADDR(0));
    acpu_volt = (800 + 8*acpu_volt_reg)*1000;
    }else{
        /*hi6553 pmu chipid*/
        acpu_volt_reg = (unsigned int)hi6xxx_pmic_reg_read(SOC_SMART_BUCK01_CTRL3_ADDR(0));
        acpu_volt = (600*1000 + 6100*acpu_volt_reg);
        if (PMU_HI6553_V110 == pmu_version){
            acpu_volt += 12200;
        }
    }
    
    /* copy memory from virt_addr & print to freqdump_buf */
    memcpy_fromio(freqdump_tmp,  (void *)freqdump_virt_addr, MEMORY_FREQDUMP_SIZE);
    pos += snprintf(freqdump_buf+pos,BUFSZ-pos,"%u|",freqdump_tmp->freq_acpu_freq_last);
    pos += snprintf(freqdump_buf+pos,BUFSZ-pos,"%u|",freqdump_tmp->freq_acpu_freq_cur);
    pos += snprintf(freqdump_buf+pos,BUFSZ-pos,"%u|",freqdump_tmp->freq_acpu_load);
    pos += snprintf(freqdump_buf+pos,BUFSZ-pos,"%u|",freqdump_tmp->freq_acpu_core_online);
    pos += snprintf(freqdump_buf+pos,BUFSZ-pos,"%u|",freqdump_tmp->freq_every_acpu_load[0]);
    pos += snprintf(freqdump_buf+pos,BUFSZ-pos,"%u|",freqdump_tmp->freq_every_acpu_load[1]);
    pos += snprintf(freqdump_buf+pos,BUFSZ-pos,"%u|",freqdump_tmp->freq_every_acpu_load[2]);
    pos += snprintf(freqdump_buf+pos,BUFSZ-pos,"%u|",freqdump_tmp->freq_every_acpu_load[3]);
    pos += snprintf(freqdump_buf+pos,BUFSZ-pos,"%u|",freqdump_tmp->freq_ccpu_freq_last);
    pos += snprintf(freqdump_buf+pos,BUFSZ-pos,"%u|",freqdump_tmp->freq_ccpu_freq_cur);
    pos += snprintf(freqdump_buf+pos,BUFSZ-pos,"%u|",freqdump_tmp->freq_ccpu_load);
    pos += snprintf(freqdump_buf+pos,BUFSZ-pos,"%u|",freqdump_tmp->freq_ddr_freq_last);
    pos += snprintf(freqdump_buf+pos,BUFSZ-pos,"%u|",freqdump_tmp->freq_ddr_freq_cur);
    pos += snprintf(freqdump_buf+pos,BUFSZ-pos,"%u|",freqdump_tmp->freq_ddr_cmd_load);
    pos += snprintf(freqdump_buf+pos,BUFSZ-pos,"%u",freqdump_tmp->freq_ddr_data_load);

    /*transfer freqdump message to user space*/
	ret = seq_printf(s,"%s\n",freqdump_buf);
    if (ret < 0)
        printk(KERN_ERR "seq_printf overflow\n");
	return 0;
    
}

static int freqdump_dbgfs_open(struct inode *inode, struct file *file)
{
    return(single_open(file, freqdump_dbgfs_read, inode->i_private));
}

static struct file_operations freqdump_fops = {
	.owner   = THIS_MODULE,
	.open    = freqdump_dbgfs_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release
};

static int __init freqdump_debugfs_init(void)
{

    struct dentry *freqdump_file;
    struct dentry *acpu_buck1;
    printk(KERN_INFO "freqdump_debugfs_init\n");
    freqdump_tmp = kzalloc(MEMORY_FREQDUMP_SIZE,GFP_KERNEL);
    if  (!freqdump_tmp){
		return -ENOMEM;
    }
    freqdump_virt_addr = ioremap(MEMORY_FREQDUMP_ADDR,MEMORY_FREQDUMP_SIZE);
    if (NULL == freqdump_virt_addr){
        kfree(freqdump_tmp);
        return -ENOMEM;
    }
	freqdump_debugfs = debugfs_create_dir("freqdump_debugfs", NULL);
	if (!freqdump_debugfs){
		return -ENOENT;
    }

	acpu_buck1 = debugfs_create_u32("acpu_voltage", 0644, freqdump_debugfs, &acpu_volt);
	if (!acpu_buck1)
		goto Fail;      

	freqdump_file = debugfs_create_file("freqdump", 0644, freqdump_debugfs, NULL, &freqdump_fops);
	if (!freqdump_file)
		goto Fail;
    pmu_version = hi6xxx_pmic_get_version();
        return 0;
Fail:
	debugfs_remove_recursive(freqdump_debugfs);
	freqdump_debugfs = NULL;
    kfree(freqdump_tmp);
    iounmap(freqdump_virt_addr);
    printk(KERN_ERR "create file fail\n");
	return -ENOENT;
}

static void __exit freqdump_debugfs_exit(void)
{
    printk(KERN_INFO "freqdump_debugfs_exit\n");

	debugfs_remove_recursive(freqdump_debugfs);
    if(freqdump_tmp){
        kfree(freqdump_tmp);
    }
    if (freqdump_virt_addr){
        iounmap(freqdump_virt_addr);
    }
        return;
}

module_init(freqdump_debugfs_init);
module_exit(freqdump_debugfs_exit);

MODULE_LICENSE("GPL");

