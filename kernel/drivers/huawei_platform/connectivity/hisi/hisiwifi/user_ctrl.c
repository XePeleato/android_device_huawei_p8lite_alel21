


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*
 * 1 Header File Including
 */
#include <linux/kobject.h>
#include <linux/device.h>
#include "common.h"
#include "hi110x.h"
#include "hwifi_utils.h"
#include "hwifi_hcc.h"
#include "hi110x_pm.h"
#include "hwifi_msg.h"
#include "hwifi_cfg80211.h"
#include "hwifi_utils.h"
#include <linux/vmalloc.h>
#include "hwifi_perform.h"
#include <linux/gpio.h>
#include "hwifi_dev_err.h"
#include "user_ctrl.h"
#include <linux/list.h>

#ifdef __ROAM__
#include "hwifi_roam_main.h"
#endif

/*1:for hcc lock, 0:for hcc unlock*/
int log_dump_flag=1;
STATIC  LIST_HEAD(hi110x_log_head);
int panic_debug = 1;
module_param(panic_debug, int, S_IRUGO|S_IWUSR);

#define DECLARE_HCC_SYS_ENTRY(name) \
STATIC ssize_t get_sys_##name(struct device *dev, struct device_attribute *attr, char *buf)\
{\
    ssize_t ret = 0;\
    struct hcc_handler* hcc = NULL;\
    if(NULL == g_hi110x_dev)\
    {\
        return snprintf(buf, PAGE_SIZE-1, "g_hi110x_dev is null!\n");\
    }\
    hcc = g_hi110x_dev->hcc;\
    ret += snprintf(buf, PAGE_SIZE-1,"%s  is %u\n", #name, hcc->sys_##name);\
    HWIFI_WARN_ON(ret > PAGE_SIZE);\
    return ret;\
}\
STATIC ssize_t set_sys_##name(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)\
{\
    ssize_t ret = 0;\
    struct hcc_handler* hcc = NULL;\
    uint32 sys_tmp = 0;\
    if(NULL == g_hi110x_dev)\
    {\
        printk(KERN_EMERG"g_hi110x_dev is null!\n");\
        return count;\
    }\
    hcc = g_hi110x_dev->hcc;\
    ret = sscanf(buf, "%u", &sys_tmp);\
    if(1 != ret)\
    {\
        printk(KERN_EMERG"Wrong input argument,except 1 argument, now get %zd\n", ret);\
        return count;\
    }\
    printk(KERN_EMERG"set %s  from %u to %u \n", #name,\
                    hcc->sys_##name,\
                    sys_tmp);\
    \
    hcc->sys_##name = sys_tmp;\
    return count;\
}\
STATIC DEVICE_ATTR(name, S_IRUGO | S_IWUSR, get_sys_##name, set_sys_##name)

#define DECLARE_HCC_LIMIT(name, trans_type, queue_type) \
STATIC ssize_t get_##name(struct device *dev, struct device_attribute *attr, char *buf)\
{\
    ssize_t ret = 0;\
    struct hcc_handler* hcc = NULL;\
    if(NULL == g_hi110x_dev)\
    {\
        return snprintf(buf, PAGE_SIZE-1, "g_hi110x_dev is null!\n");\
    }\
    hcc = g_hi110x_dev->hcc;\
    ret += snprintf(buf, PAGE_SIZE-1, "tx %-30s limit %u  0x%X\n", \
                        hcc_get_queue_str(queue_type), hcc->hcc_queues[trans_type].queues[queue_type].queue_trans_limit, hcc->hcc_queues[trans_type].queues[queue_type].queue_trans_limit);\
    HWIFI_WARN_ON(ret > PAGE_SIZE);\
    return ret;\
}\
STATIC ssize_t set_##name(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)\
{\
    ssize_t ret = 0;\
    struct hcc_handler* hcc = NULL;\
    uint32 limit = 0xffffffff;\
    if(NULL == g_hi110x_dev)\
    {\
        printk(KERN_EMERG"g_hi110x_dev is null!\n");\
        return count;\
    }\
    hcc = g_hi110x_dev->hcc;\
    ret = sscanf(buf, "%u", &limit);\
    if(1 != ret)\
    {\
        printk(KERN_EMERG"Wrong input argument,except 1 argument, now get %zd\n", ret);\
        return count;\
    }\
    if(!limit)\
    {\
        printk(KERN_EMERG"wrong limit, can't set limit to 0 !!\n");\
        return count;\
    } \
    printk(KERN_EMERG"set %s queue %s limit from %u to %u \n", (trans_type == HCC_TX)? "tx ":"rx ",\
                    hcc_get_queue_str(queue_type),\
                    hcc->hcc_queues[trans_type].queues[queue_type].queue_trans_limit,\
                    limit);\
    \
    hcc_set_trans_limit(hcc, trans_type,queue_type , limit);\
    return count;\
}\
STATIC DEVICE_ATTR(name, S_IRUGO | S_IWUSR, get_##name, set_##name)
#define DECLARE_HCC_QUOTIETY(name) \
STATIC ssize_t get_tcp_##name(struct device *dev, struct device_attribute *attr, char *buf)\
{\
    ssize_t ret = 0;\
    struct hcc_handler* hcc = NULL;\
    if(NULL == g_hi110x_dev)\
    {\
        return snprintf(buf, PAGE_SIZE-1, "g_hi110x_dev is null!\n");\
    }\
    hcc = g_hi110x_dev->hcc;\
    ret += snprintf(buf, PAGE_SIZE-1,"%s  is %u\n", #name, hcc->tcp_##name);\
    HWIFI_WARN_ON(ret > PAGE_SIZE);\
    return ret;\
}\
STATIC ssize_t set_tcp_##name(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)\
{\
    ssize_t ret = 0;\
    struct hcc_handler* hcc = NULL;\
    uint32 quotiety = 0;\
    if(NULL == g_hi110x_dev)\
    {\
        printk(KERN_EMERG"g_hi110x_dev is null!\n");\
        return count;\
    }\
    hcc = g_hi110x_dev->hcc;\
    ret = sscanf(buf, "%u", &quotiety);\
    if(1 != ret)\
    {\
        printk(KERN_EMERG"Wrong input argument,except 1 argument, now get %zd\n", ret);\
        return count;\
    }\
    if(!quotiety)\
    {\
        printk(KERN_EMERG"wrong quotiety, can't set quotiety to 0 !!\n");\
        return count;\
    } \
    printk(KERN_EMERG"set %s  from %u to %u \n", #name,\
                    hcc->tcp_##name,\
                    quotiety);\
    \
    hcc->tcp_##name = quotiety;\
    return count;\
}\
STATIC DEVICE_ATTR(tcp_##name, S_IRUGO | S_IWUSR, get_tcp_##name, set_tcp_##name)
/*
 * 2 Global Variable Definition
 */

struct kobject *g_sysfs_hi110x = NULL;
struct kobject *hcc_obj = NULL;
extern int32 exception_enable;
int32 g_certify_powermode = 0xFF;
int32 g_high_throughput_mode     = 0;
int32 g_normal_mode = 1;
int32 g_sleep_policy    = 0xFF;

/*
 * 3 Function Definition
 */


STATIC ssize_t get_loglevel(struct device *dev, struct device_attribute *attr, char *buf)
{
    return snprintf(buf, PAGE_SIZE-1, "current_loglevel = %d\n", loglevel);
}


STATIC ssize_t set_loglevel(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int32 param_cnt;
    int32 level;

    param_cnt = sscanf(buf, "%d", &level);
    if (param_cnt != 1)
    {
        printk(KERN_EMERG"error param\n");

        return -EFAIL;
    }
    printk(KERN_EMERG"\n");

    loglevel = level;
    printk(KERN_EMERG"set loglevel=%u\n", loglevel);
    return count;
}


STATIC DEVICE_ATTR(loglevel, S_IRUGO | S_IWUSR, get_loglevel, set_loglevel);


STATIC ssize_t hwifi_get_coex_level(struct device *dev, struct device_attribute *attr, char *buf)
{
    return snprintf(buf, PAGE_SIZE-1, "current_coex_level = %d\n", g_coex_level);
}


STATIC ssize_t hwifi_set_coex_level(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int32 param_cnt;
    int32 level;

    param_cnt = sscanf(buf, "%d", &level);
    if (param_cnt != 1)
    {
        printk(KERN_EMERG"error param\n");

        return count;
    }
    printk(KERN_EMERG"\n");

    g_coex_level = level;
    printk(KERN_EMERG"set coex level=%u\n", g_coex_level);
    return count;
}


STATIC DEVICE_ATTR(g_coex_level, S_IRUGO | S_IWUSR, hwifi_get_coex_level, hwifi_set_coex_level);
STATIC struct attribute *g_coex_level_entity[] = {
    &dev_attr_g_coex_level.attr,
    NULL
};
STATIC struct attribute_group g_coex_level_attribute_group = {
    .name = NULL,
    .attrs = g_coex_level_entity,
};

void hwifi_union_log_dump(char* print_level)
{
    hwifi_union_log* log = NULL;
    struct list_head* head = &hi110x_log_head;
    print_level = print_level?:"";    
    printk("%s\n",print_level);
    list_for_each_entry(log,head, list)
    {
        printk("%s[%s]:\n",print_level, log->module);
        log->loglevel = print_level;
        log->cb(log);
        printk("%s\n",print_level);
    }
    printk("%s\n",print_level);
}

static int hwifi_panic_handler(struct notifier_block *this,
                   unsigned long event, void *unused)
{
    if(panic_debug)
        hwifi_union_log_dump(KERN_DEBUG);
    else
        printk(KERN_DEBUG"wifi panic debug off\n");
    return NOTIFY_OK;
}
static struct notifier_block hwifi_panic_notifier = {
    .notifier_call  = hwifi_panic_handler,
    .next           = NULL,
    .priority       = 160   /* priority: INT_MAX >= x >= 0 */
};
STATIC ssize_t get_assem_info(struct device *dev, struct device_attribute *attr, char *buf)
{
    int i;
    ssize_t ret = 0;
    unsigned long total_len = 0;
    uint32 tx_assem_count = sizeof(g_hi110x_dev->hcc->assem_tx_count)/sizeof(uint32);
    if(NULL == g_hi110x_dev)
    {
        return snprintf(buf, PAGE_SIZE-1, "g_hi110x_dev is null!\n");
    }
    ret += snprintf(buf + ret, PAGE_SIZE-1, "usage:clear asseminfo\necho 1 > assem_info\n");
    ret += snprintf(buf + ret, PAGE_SIZE-1, "[r][0][4]\n r/w: rx/tx\n [0]: 0,single pkt,1-%u assem nums \n [4]:pkt numbers\n",
                    tx_assem_count ? tx_assem_count -1 : 0);
    ret += snprintf(buf + ret, PAGE_SIZE-1, "tx assem info:\n");
    spin_lock(&g_hi110x_dev->hcc->assem_c_lock);
    for(i = 0;i < sizeof(g_hi110x_dev->hcc->assem_tx_count)/sizeof(uint32);i++)
    {
        ret += snprintf(buf + ret, PAGE_SIZE-1, "[%s][%2.2d][%d]\n", i?"a":"x", i, g_hi110x_dev->hcc->assem_tx_count[i]);
        total_len += (i?:1)*g_hi110x_dev->hcc->assem_tx_count[i];
    }
    ret += snprintf(buf + ret, PAGE_SIZE-1, "tx total:%lu\n", total_len);
    total_len = 0;
    ret += snprintf(buf + ret, PAGE_SIZE-1, "rx assem info:\n");
    for(i = 0;i < sizeof(g_hi110x_dev->hcc->assem_rx_count)/sizeof(uint32);i++)
    {
        ret += snprintf(buf + ret, PAGE_SIZE-1, "[%s][%2.2d][%d]\n", i?"a":"x", i, g_hi110x_dev->hcc->assem_rx_count[i]);
        total_len += (i?:1)*g_hi110x_dev->hcc->assem_rx_count[i];
    }
    spin_unlock(&g_hi110x_dev->hcc->assem_c_lock);
    ret += snprintf(buf + ret, PAGE_SIZE-1, "rx total:%lu\n", total_len);
    HWIFI_WARN_ON(ret > PAGE_SIZE);
    return ret;
}

STATIC ssize_t set_assem_info(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    assem_debug_count_reset(g_hi110x_dev->hcc);
    printk(KERN_EMERG"reset assem debug info done.\n");
    return count;
}
STATIC DEVICE_ATTR(assem_info, S_IRUGO | S_IWUSR, get_assem_info, set_assem_info);

static void hwifi_union_log_worker(struct work_struct *work)
{
    if(!g_hi110x_dev ||! g_hi110x_dev->hcc)
    {
        printk(KERN_EMERG"ptr is null!\n");
        return;
    }
    if(log_dump_flag)
        hcc_transfer_lock(g_hi110x_dev->hcc);
    hwifi_union_log_dump(KERN_DEBUG);
    if(log_dump_flag)
        hcc_transfer_unlock(g_hi110x_dev->hcc);    
}
DECLARE_WORK(union_log_worker, hwifi_union_log_worker);

struct work_struct *hwifi_union_get_worker(void)
{
    return &union_log_worker;
}

STATIC ssize_t set_dump(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int flag = 0;
    int32 param_cnt;

    param_cnt = sscanf(buf, "%d", &flag);
    if (param_cnt != 1)
    {
        printk(KERN_EMERG"error param\n");
        return count;
    }

    log_dump_flag = !!flag;
    printk(KERN_EMERG"%s hcc lock when dump message!\n", log_dump_flag?"add":"do not add");
     return count;
}

STATIC ssize_t get_dump(struct device *dev, struct device_attribute *attr, char *buf)
{
    ssize_t ret = 0;
    if(NULL == g_hi110x_dev)
    {
        return snprintf(buf, PAGE_SIZE-1, "g_hi110x_dev is null!\n");
    }
    ret += snprintf(buf + ret, PAGE_SIZE-1, "dump hcc chan full info into kmsg!\n");
    if(log_dump_flag)
        hcc_transfer_lock(g_hi110x_dev->hcc);
    hwifi_union_log_dump(KERN_EMERG);
    if(log_dump_flag)    
        hcc_transfer_unlock(g_hi110x_dev->hcc);
    return ret;
}

STATIC DEVICE_ATTR(dump,  S_IRUGO | S_IWUSR, get_dump, set_dump);
void debug_power_reset_test(void)
{
    return;
//    printk(KERN_EMERG"Force to pull down wlan en\n");
  //  gpio_set_value(GPIO_0_2, 0);
   // mdelay(10);
   // gpio_set_value(GPIO_0_2, 1);
}

void debug_exception_test(void)
{
    hwifi_exception_force_submit(g_hi110x_dev->hcc, "debug");
}

void debug_devicemem_test(void)
{
    HWIFI_WARNING("Start to export device mem!This will cause device reboot!");
    debug_exception_test();
    HWIFI_WARNING("Export Done.Please check /data/memdump");
}
void debug_devicemem_forcedump(void)
{
    int     wlan_stat;
    HWIFI_INFO("Froce to dump device mem");
    hcc_set_tx_queue_status(g_hi110x_dev->hcc, hcc_abort);
    bus_set_state(g_hi110x_dev->hcc->io_dev, FALSE);
    wlan_opera_lock(g_hi110x_dev->pm_info);
    hwifi_powerpin_state_save(&wlan_stat);
    hwifi_wlan_close(g_hi110x_dev->netdev);
    hwifi_wlan_open(g_hi110x_dev->netdev);
    hwifi_powerpin_state_restore(wlan_stat);
    wlan_opera_unlock(g_hi110x_dev->pm_info);  
    HWIFI_INFO("Dump device mem done.");
}

void debug_device_flowctrl(void)
{   
    printk(KERN_EMERG"Test case: force to close device flow ctrl");
    hcc_dev_flowctrl_off(g_hi110x_dev->hcc, HWIFI_DEV2HOST);
}


void debug_driver_state_test(void)
{
    printk(KERN_EMERG"submit a net work excetpion\n");
    mod_timer(&g_hi110x_dev->hcc->net_work_detect.detect_timer, jiffies);
}

static user_ctrl_debug debug_items[DEBUG_ITEM_COUNT] =
{
    [DEBUG_POWER_OFF]        ={"powereset", "Force to reset wlan_en, make sure power on gpio keep high",
                               debug_power_reset_test},
    [DEBUG_EXCEPTION_SUMBIT] ={"exception-test", "Submit a exception manually to test exception process",
                               debug_exception_test},
    [DEBUG_DEVICE_MEM] ={"devicemem", "export the device mem to /data/memdump just for sta mode!!",
                               debug_devicemem_test}, 
    [DEBUG_DEVICE_MEM_FROCE] = {"devmem_force", "force to dump device mem, we should use devicemem first!",
                               debug_devicemem_forcedump},      
    [DEBUG_DEVICE_FLOWCTRL] = {"dev_flowctrl", "force to lock dev flow ctrl , this should cause a dev flow ctrl dead lock manually",
                               debug_device_flowctrl},
    [DEBUG_NET_WORK_EXCETPION] = {"driver_state", "test case for driver state test, net work excetpion simulator",
                                debug_driver_state_test},
};

STATIC ssize_t set_debug(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int i = 0;
    char cmd[128] = {0};
    if(NULL == g_hi110x_dev)
    {
        printk(KERN_EMERG"g_hi110x_dev is null!\n");
        return count;
    }
    if(1 != sscanf(buf, "%s", cmd))
    {
        printk(KERN_EMERG"only support one commond ,your input:[%s]\n",cmd);
        return count;
    }

    for(i = 0; i < DEBUG_ITEM_COUNT; i++)
    {
        if(!strcmp(cmd, debug_items[i].name))
        {
            break;
        }
    }
    
    if(i == DEBUG_ITEM_COUNT)
    {
        printk(KERN_EMERG"Unsupport Command : %s \n", cmd);
        return count;        
    }

    if(debug_items[i].func)
    {
        debug_items[i].func();
    }
    else
    {
        printk(KERN_EMERG"%s had no operation func\n", debug_items[i].name);
    }
    return count;
}

STATIC ssize_t get_debug(struct device *dev, struct device_attribute *attr, char *buf)
{
    int i = 0;
    ssize_t  ret = 0;
    if(NULL == g_hi110x_dev)
    {
        return snprintf(buf, PAGE_SIZE-1, "g_hi110x_dev is null!\n");
    }
    ret += snprintf(buf + ret, PAGE_SIZE-1, "How To Debug: echo commond > /sys/hi110x/debug!\n");
    ret += snprintf(buf + ret, PAGE_SIZE-1, "Support Debug Command:\n");
    for(i = 0; i < DEBUG_ITEM_COUNT; i++)
    {
        ret += snprintf(buf + ret, PAGE_SIZE-1, "  %s:\n",  debug_items[i].name);
        ret += snprintf(buf + ret, PAGE_SIZE-1, "    %s\n", debug_items[i].usage);
    }
    return ret;
}
STATIC DEVICE_ATTR(debug,  S_IRUGO | S_IWUSR, get_debug, set_debug);

/*The net work detect Interface with Android.*/
STATIC ssize_t set_driver_state(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int enable;
    unsigned long seconds;
    if(NULL == g_hi110x_dev)
    {
        printk(KERN_EMERG"g_hi110x_dev is null!\n");
        return count;
    }
    if(sscanf(buf, "%d %lu", &enable, &seconds) > 2)
    {
        printk(KERN_EMERG"only support one commond ,your input:[%s]\n",buf);
        return -EINVAL;
    }
    HWIFI_ERROR("enable change[%d]==>[%d], timeout chagne[%lu]==>[%lu]",
                                g_hi110x_dev->hcc->net_work_detect.enable, enable,
                                g_hi110x_dev->hcc->net_work_detect.timeout/HZ, seconds);
    if(enable)
        g_hi110x_dev->hcc->net_work_detect.timeout = seconds*HZ;
    g_hi110x_dev->hcc->net_work_detect.enable = !!enable;
    return count;
}

STATIC ssize_t get_driver_state(struct device *dev, struct device_attribute *attr, char *buf)
{
    int state;
    ssize_t ret = 0;
    if(!g_hi110x_dev || !g_hi110x_dev->hcc)
    {
        state = 0;
    }
    else
    {
        state = g_hi110x_dev->hcc->net_work_detect.driver_state;
    }
    ret += snprintf(buf + ret, PAGE_SIZE-1, "recovery_count=%d\n", state);
    return ret;
}
STATIC DEVICE_ATTR(driver_state,   S_IRUGO | S_IWUSR , get_driver_state, set_driver_state);


STATIC ssize_t get_roam_status(struct device *dev, struct device_attribute *attr, char *buf)
{
    int32 state;
    ssize_t ret = 0;
    if(!g_hi110x_dev || !g_hi110x_dev->cfg)
    {
        state = 0;
    }
    else
    {
    #ifdef __ROAM__
        state = IS_ROAMING_STATE(g_hi110x_dev->cfg->roam.roam_fsm.state);
    #else
        state = 0;
    #endif
    }
    
    ret += snprintf(buf + ret, PAGE_SIZE-1, "roam_status=%d\n", state);
    return ret;
}
STATIC DEVICE_ATTR(roam_status,   S_IRUGO | S_IWUSR , get_roam_status, NULL);


STATIC ssize_t set_roam_scan_interval(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int32 min_scan_interval_1;
    int32 min_scan_interval_2;
    int32 min_scan_interval_3;
    int32 min_scan_interval_4;
    

    if(NULL == g_hi110x_dev)
    {
        HWIFI_WARNING("g_hi110x_dev is null!\n");
        return count;
    }
    if(sscanf(buf, "%d %d %d %d", &min_scan_interval_1, &min_scan_interval_2,
                            &min_scan_interval_3, &min_scan_interval_4) != 4)
    {
        HWIFI_WARNING("should set 4 argument,min_scan_interval_1 ~ min_scan_interval_4(-80,-75,-70,>-70)\n");
        HWIFI_WARNING("an example is :echo 10 15 20 60 > roam_scan_interval\n");
        return -EINVAL;
    }
    
#ifdef __ROAM__
    g_hi110x_dev->cfg->roam.roam_fsm.min_scan_interval_1 = min_scan_interval_1;
    g_hi110x_dev->cfg->roam.roam_fsm.min_scan_interval_2 = min_scan_interval_2;
    g_hi110x_dev->cfg->roam.roam_fsm.min_scan_interval_3 = min_scan_interval_3;
    g_hi110x_dev->cfg->roam.roam_fsm.min_scan_interval_4 = min_scan_interval_4;
#endif    

    return count;
}

STATIC ssize_t get_roam_scan_interval(struct device *dev, struct device_attribute *attr, char *buf)
{
    ssize_t ret = 0;
    if(!g_hi110x_dev)
    {
        return ret;
    }
    
#ifdef __ROAM__    
    ret = snprintf(buf, PAGE_SIZE-1, "min_scan_interval_1-4 is %d,%d,%d,%d\n", 
                                        g_hi110x_dev->cfg->roam.roam_fsm.min_scan_interval_1,
                                        g_hi110x_dev->cfg->roam.roam_fsm.min_scan_interval_2,
                                        g_hi110x_dev->cfg->roam.roam_fsm.min_scan_interval_3,
                                        g_hi110x_dev->cfg->roam.roam_fsm.min_scan_interval_4);
#endif    
    return ret;
}
STATIC DEVICE_ATTR(roam_scan_interval,   S_IRUGO | S_IWUSR , get_roam_scan_interval, set_roam_scan_interval);


STATIC ssize_t set_roam_min_diff_rssi(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int32 min_diff_rssi_1;
    int32 min_diff_rssi_2;
    int32 min_diff_rssi_3;
    int32 min_diff_rssi_4;
    

    if(NULL == g_hi110x_dev)
    {
        HWIFI_WARNING("g_hi110x_dev is null!\n");
        return count;
    }
    if(sscanf(buf, "%d %d %d %d", &min_diff_rssi_1, &min_diff_rssi_2,
                            &min_diff_rssi_3, &min_diff_rssi_4) != 4)
    {
        HWIFI_WARNING("should set 4 argument,min_diff_rssi_1 ~ min_diff_rssil_4(-80,-75,-70,>-70)\n");
        HWIFI_WARNING("an example is :echo 4 6 8 10 > min_diff_rssi\n");
        return -EINVAL;
    }
    
#ifdef __ROAM__
    g_hi110x_dev->cfg->roam.roam_fsm.min_diff_rssi_1 = min_diff_rssi_1;
    g_hi110x_dev->cfg->roam.roam_fsm.min_diff_rssi_2 = min_diff_rssi_2;
    g_hi110x_dev->cfg->roam.roam_fsm.min_diff_rssi_3 = min_diff_rssi_3;
    g_hi110x_dev->cfg->roam.roam_fsm.min_diff_rssi_4 = min_diff_rssi_4;
#endif    

    return count;
}

STATIC ssize_t get_roam_min_diff_rssi(struct device *dev, struct device_attribute *attr, char *buf)
{
    ssize_t ret = 0;
    if(!g_hi110x_dev)
    {
        return ret;
    }
    
#ifdef __ROAM__    
    ret = snprintf(buf, PAGE_SIZE-1, "min_diff_rssi_1-4 is %d,%d,%d,%d\n", 
                                        g_hi110x_dev->cfg->roam.roam_fsm.min_diff_rssi_1,
                                        g_hi110x_dev->cfg->roam.roam_fsm.min_diff_rssi_2,
                                        g_hi110x_dev->cfg->roam.roam_fsm.min_diff_rssi_3,
                                        g_hi110x_dev->cfg->roam.roam_fsm.min_diff_rssi_4);
#endif    
    return ret;
}
STATIC DEVICE_ATTR(roam_min_diff_rssi, S_IRUGO | S_IWUSR, get_roam_min_diff_rssi, set_roam_min_diff_rssi);

STATIC ssize_t set_roam_tx_packet_failed_param(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int32 time_interval;
    int32 rssi_threshold;
    int32 tx_failed_scale_threshold;
    uint32 tx_packet_num_threshold;
    int32 tx_rate_threshold;

    if(NULL == g_hi110x_dev)
    {
        HWIFI_WARNING("g_hi110x_dev is null!\n");
        return count;
    }
    if(sscanf(buf, "%d %d %d %u %d", &time_interval, &rssi_threshold, &tx_failed_scale_threshold, &tx_packet_num_threshold, &tx_rate_threshold) != 5)
    {
        HWIFI_WARNING("should set 5 argument,time_interval,rssi_threshold,tx_failed_scale_threshold,tx_rate_threshold\n");
        HWIFI_WARNING("an example is :echo 15 -70 70 20 6 > roam_tx_packet_failed_param\n");
        return -EINVAL;
    }

#ifdef __ROAM__
    g_hi110x_dev->cfg->roam.alg_param.tx_pkt_param.time_interval = time_interval;
    g_hi110x_dev->cfg->roam.alg_param.tx_pkt_param.rssi_threshold = rssi_threshold;
    g_hi110x_dev->cfg->roam.alg_param.tx_pkt_param.tx_failed_scale_threshold = tx_failed_scale_threshold;
    g_hi110x_dev->cfg->roam.alg_param.tx_pkt_param.tx_packet_num_threshold = tx_packet_num_threshold;
    g_hi110x_dev->cfg->roam.alg_param.tx_pkt_param.tx_rate_threshold = tx_rate_threshold;
#endif

    return count;
}
STATIC ssize_t get_roam_tx_packet_failed_param(struct device *dev, struct device_attribute *attr, char *buf)
{
    ssize_t ret = 0;
    if(!g_hi110x_dev)
    {
        return ret;
    }

#ifdef __ROAM__
    ret = snprintf(buf, PAGE_SIZE-1, "tx_pkt_param is time_interval:%d, rssi_threshold:%d,tx_failed_scale_threshold:%d,tx_packet_num_threshold:%u,tx_rate_threshold=%d\n",
                                        g_hi110x_dev->cfg->roam.alg_param.tx_pkt_param.time_interval,
                                        g_hi110x_dev->cfg->roam.alg_param.tx_pkt_param.rssi_threshold,
                                        g_hi110x_dev->cfg->roam.alg_param.tx_pkt_param.tx_failed_scale_threshold,
                                        g_hi110x_dev->cfg->roam.alg_param.tx_pkt_param.tx_packet_num_threshold,
                                        g_hi110x_dev->cfg->roam.alg_param.tx_pkt_param.tx_rate_threshold);
#endif
    return ret;
}
STATIC DEVICE_ATTR(roam_tx_packet_failed_param, S_IRUGO | S_IWUSR, get_roam_tx_packet_failed_param, set_roam_tx_packet_failed_param);

STATIC struct attribute *loglevel_entity[] = {
    &dev_attr_loglevel.attr,
    &dev_attr_dump.attr,
    &dev_attr_debug.attr,   
    &dev_attr_assem_info.attr,
    &dev_attr_driver_state.attr,
    &dev_attr_roam_status.attr,
    &dev_attr_roam_scan_interval.attr,
    &dev_attr_roam_min_diff_rssi.attr,
    &dev_attr_roam_tx_packet_failed_param.attr,
    NULL
};
STATIC struct attribute_group g_loglevel_attribute_group = {
    .name = NULL,
    .attrs = loglevel_entity,
};


STATIC ssize_t get_powermgmt_switch(struct device *dev, struct device_attribute *attr, char *buf)
{
    return snprintf(buf, PAGE_SIZE-1, "current_powermgmt_switch = %d\n", g_powermgmt_switch);
}


STATIC ssize_t set_powermgmt_switch(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int32 param_cnt;
    int32 powermgmt_switch;

    param_cnt = sscanf(buf, "%d", &powermgmt_switch);
    if (param_cnt != 1)
    {
        printk(KERN_EMERG"error param\n");

        return count;
    }
    printk(KERN_INFO"\n");

    g_powermgmt_switch = powermgmt_switch;

    return count;
}

STATIC DEVICE_ATTR(powermgmt_switch, S_IRUGO | S_IWUSR, get_powermgmt_switch, set_powermgmt_switch);
STATIC struct attribute *powermgmt_switch_entity[] = {
    &dev_attr_powermgmt_switch.attr,
    NULL
};
STATIC struct attribute_group g_powermgmt_switch_attribute_group = {
    .name = NULL,
    .attrs = powermgmt_switch_entity,
};

#ifdef WMM_OPT_FOR_AUTH
STATIC ssize_t get_wmm_drop_interval(struct device *dev, struct device_attribute *attr, char *buf)
{
    return snprintf(buf, PAGE_SIZE-1, "current_wmm_drop_interval = %d\n", g_wmm_drop_interval);
}

STATIC ssize_t set_wmm_drop_interval(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int32 param_cnt;
    int32 wmm_drop_interval;

    param_cnt = sscanf(buf, "%d", &wmm_drop_interval);
    if (param_cnt != 1)
    {
        printk(KERN_EMERG"error param\n");

        return count;
    }
    printk(KERN_INFO"\n");

    g_wmm_drop_interval = wmm_drop_interval;

    return count;
}

STATIC DEVICE_ATTR(wmm_drop_interval, S_IRUGO | S_IWUSR, get_wmm_drop_interval, set_wmm_drop_interval);

STATIC ssize_t get_wmm_que_len_for_drop(struct device *dev, struct device_attribute *attr, char *buf)
{
    return snprintf(buf, PAGE_SIZE-1, "current_wmm_que_len_for_drop = %d\n", g_wmm_que_len_for_drop);
}

STATIC ssize_t set_wmm_que_len_for_drop(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int32 param_cnt;
    int32 wmm_que_len_for_drop;

    param_cnt = sscanf(buf, "%d", &wmm_que_len_for_drop);
    if (param_cnt != 1)
    {
        printk(KERN_EMERG"error param\n");

        return count;
    }
    printk(KERN_INFO"\n");

    g_wmm_que_len_for_drop = wmm_que_len_for_drop;

    return count;
}

STATIC DEVICE_ATTR(wmm_que_len_for_drop, S_IRUGO | S_IWUSR, get_wmm_que_len_for_drop, set_wmm_que_len_for_drop);

STATIC ssize_t get_wmm_drop_cnt_per_time(struct device *dev, struct device_attribute *attr, char *buf)
{
    return snprintf(buf, PAGE_SIZE-1, "current_wmm_drop_cnt_per_time = %d\n", g_wmm_drop_cnt_per_time);
}

STATIC ssize_t set_wmm_drop_cnt_per_time(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int32 param_cnt;
    int32 wmm_drop_cnt_per_time;

    param_cnt = sscanf(buf, "%d", &wmm_drop_cnt_per_time);
    if (param_cnt != 1)
    {
        printk(KERN_EMERG"error param\n");

        return count;
    }
    printk(KERN_INFO"\n");

    g_wmm_drop_cnt_per_time = wmm_drop_cnt_per_time;

    return count;
}

STATIC DEVICE_ATTR(wmm_drop_cnt_per_time, S_IRUGO | S_IWUSR, get_wmm_drop_cnt_per_time, set_wmm_drop_cnt_per_time);

#endif


STATIC ssize_t get_certify_powermode(struct device *dev, struct device_attribute *attr, char *buf)
{
    return snprintf(buf, PAGE_SIZE-1, "current_certify powermode = %d\n", g_certify_powermode);
}


STATIC ssize_t set_certify_powermode(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int32 param_cnt;
    int32 powermode;

    param_cnt = sscanf(buf, "%d", &powermode);
    if (param_cnt != 1)
    {
        printk(KERN_EMERG"error param\n");

        return count;
    }
    printk(KERN_INFO"\n");

    /* 认证模式下
     * 1、设置关闭低功耗
     * 2、配置协议节能
     * 3、配置240MHz CPU频率
     */
     if (g_hi110x_dev->cfg != NULL)
     {
         if (IS_CONNECTED(g_hi110x_dev->cfg))
         {
             g_powermgmt_switch = 0;
             g_certify_powermode = powermode;

            if (powermode)
            {
                /* 关闭hips开关 */
                hwifi_set_hips_switch(g_hi110x_dev->cfg, 0);
            }
            else
            {
                /* 重新打开hips开关 */
                hwifi_set_hips_switch(g_hi110x_dev->cfg, 1);
            }
             hwifi_set_powermode(g_hi110x_dev->cfg, powermode);

             /* 如果配置省电模式0，配置Device相应的模式 */
             if (0 == powermode)
             {
                 HWIFI_INFO("shutdown powersave now...");
                 hwifi_shutdown_powersave(g_hi110x_dev->cfg);
             }

             hwifi_fit_cpufreq(g_hi110x_dev->cfg, WLCPU_240MHZ);
         }
         else
         {
            HWIFI_WARNING("please make sure connected now");
         }
     }
     else
     {
         HWIFI_WARNING("cfg ptr is NULL...");
     }

    return count;
}

STATIC DEVICE_ATTR(certify_powermode, S_IRUGO | S_IWUSR, get_certify_powermode, set_certify_powermode);
STATIC ssize_t get_high_throughput_mode(struct device *dev, struct device_attribute *attr, char *buf)
{
    return snprintf(buf, PAGE_SIZE-1, "[high throughput mode:] current throughput mode = %d\n", g_high_throughput_mode);
}


STATIC ssize_t set_high_throughput_mode(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int32 param_cnt;
    int32 high_throughput_mode;

    param_cnt = sscanf(buf, "%d", &high_throughput_mode);
    if (param_cnt != 1)
    {
        printk(KERN_EMERG"error param\n");

        return count;
    }
    printk(KERN_INFO"\n");

    /* 性能模式下
     * 1、设置关闭低功耗
     * 2、设置关闭协议节能
     * 3、配置240MHz CPU频率
     */
     if (g_hi110x_dev->cfg != NULL)
     {
         if (high_throughput_mode)
         {
             g_powermgmt_switch = 0;
             g_high_throughput_mode = high_throughput_mode;
             hwifi_set_powermode(g_hi110x_dev->cfg, 0);
             hwifi_fit_cpufreq(g_hi110x_dev->cfg, WLCPU_240MHZ);
         }
     }
     else
     {
        HWIFI_WARNING("cfg ptr is NULL...");
     }

    return count;
}

STATIC DEVICE_ATTR(high_throughput_mode, S_IRUGO | S_IWUSR, get_high_throughput_mode, set_high_throughput_mode);



STATIC ssize_t get_normal_mode(struct device *dev, struct device_attribute *attr, char *buf)
{
    return snprintf(buf, PAGE_SIZE-1, "[Normal mode:] current powermode_switch = %d\n", g_normal_mode);
}


STATIC ssize_t set_normal_mode(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int32 param_cnt;
    int32 normal_mode;

    param_cnt = sscanf(buf, "%d", &normal_mode);
    if (param_cnt != 1)
    {
        printk(KERN_EMERG"error param\n");

        return count;
    }
    printk(KERN_INFO"\n");

    /* 正常模式下
     * 1、设置打开低功耗
     * 2、协议节能根据具体应用场景设置
     * 3、配置240MHz CPU频率
     */
     if (g_hi110x_dev->cfg != NULL)
     {
         if (normal_mode)
         {
             g_powermgmt_switch = 1;
             g_normal_mode = normal_mode;
             hwifi_fit_cpufreq(g_hi110x_dev->cfg, WLCPU_240MHZ);
         }
     }
     else
     {
        HWIFI_WARNING("cfg ptr is NULL...");
     }

    return count;
}

STATIC DEVICE_ATTR(normal_mode, S_IRUGO | S_IWUSR, get_normal_mode, set_normal_mode);
STATIC struct attribute *perform_mode[] = {
    &dev_attr_certify_powermode.attr,
    &dev_attr_high_throughput_mode.attr,
    &dev_attr_normal_mode.attr,
    NULL
};
STATIC struct attribute_group perform_mode_attribute_group = {
    .name = "perform_mode",
    .attrs = perform_mode,
};


STATIC ssize_t get_cur_cpufreq(struct device *dev, struct device_attribute *attr, char *buf)
{
    return snprintf(buf, PAGE_SIZE-1, "HI1101 current static cpu freqency = %d\n", g_cur_cpufreq);
}


STATIC ssize_t set_cur_cpufreq(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int32 param_cnt;
    int32 cur_cpufreq;

    param_cnt = sscanf(buf, "%d", &cur_cpufreq);
    if (param_cnt != 1)
    {
        printk(KERN_EMERG"error param\n");

        return count;
    }
    printk(KERN_INFO"\n");


    /* 如果命令行配置cpu频率 */
   printk(KERN_ERR"fit cpu frequency by user object");
   if (cur_cpufreq)
   {
        hwifi_fit_cpufreq(g_hi110x_dev->cfg, cur_cpufreq);
        g_cur_cpufreq = cur_cpufreq;
   }

    return count;
}

STATIC DEVICE_ATTR(cur_cpufreq, S_IRUGO | S_IWUSR, get_cur_cpufreq, set_cur_cpufreq);
STATIC struct attribute *cur_cpufreq_entity[] = {
    &dev_attr_cur_cpufreq.attr,
    NULL
};
STATIC struct attribute_group g_cur_cpufreq_attribute_group = {
    .name = NULL,
    .attrs = cur_cpufreq_entity,
};
STATIC ssize_t get_40M_sdio_rate(struct device *dev, struct device_attribute *attr, char *buf)
{
    return snprintf(buf, PAGE_SIZE-1, "HI1101 40MHz cpu freqency sdio_rate = %d\n", g_sdio_rate_40M);
}


STATIC ssize_t set_40M_sdio_rate(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int32 param_cnt;
    int32 sdio_rate_40M;

    param_cnt = sscanf(buf, "%d", &sdio_rate_40M);
    if (param_cnt != 1)
    {
        printk(KERN_EMERG"error param\n");

        return count;
    }
    printk(KERN_INFO"\n");

    g_sdio_rate_40M = sdio_rate_40M;

    return count;
}

STATIC DEVICE_ATTR(sdio_rate_40M, S_IRUGO | S_IWUSR, get_40M_sdio_rate, set_40M_sdio_rate);


STATIC ssize_t get_80M_sdio_rate(struct device *dev, struct device_attribute *attr, char *buf)
{
    return snprintf(buf, PAGE_SIZE-1, "HI1101 80MHz cpu freqency sdio_rate = %d\n", g_sdio_rate_80M);
}


STATIC ssize_t set_80M_sdio_rate(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int32 param_cnt;
    int32 sdio_rate_80M;

    param_cnt = sscanf(buf, "%d", &sdio_rate_80M);
    if (param_cnt != 1)
    {
        printk(KERN_EMERG"error param\n");

        return count;
    }
    printk(KERN_INFO"\n");

    g_sdio_rate_80M = sdio_rate_80M;

    return count;
}

STATIC DEVICE_ATTR(sdio_rate_80M, S_IRUGO | S_IWUSR, get_80M_sdio_rate, set_80M_sdio_rate);


STATIC ssize_t get_160M_sdio_rate(struct device *dev, struct device_attribute *attr, char *buf)
{
    return snprintf(buf, PAGE_SIZE-1, "HI1101 160MHz cpu freqency sdio_rate = %d\n", g_sdio_rate_160M);
}


STATIC ssize_t set_160M_sdio_rate(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int32 param_cnt;
    int32 sdio_rate_160M;

    param_cnt = sscanf(buf, "%d", &sdio_rate_160M);
    if (param_cnt != 1)
    {
        printk(KERN_EMERG"error param\n");

        return count;
    }
    printk(KERN_INFO"\n");

    g_sdio_rate_160M = sdio_rate_160M;

    return count;
}

STATIC DEVICE_ATTR(sdio_rate_160M, S_IRUGO | S_IWUSR, get_160M_sdio_rate, set_160M_sdio_rate);

STATIC struct attribute *cpufreq_ajust_threshold[] = {
    &dev_attr_sdio_rate_40M.attr,
    &dev_attr_sdio_rate_80M.attr,
    &dev_attr_sdio_rate_160M.attr,
    NULL
};
STATIC struct attribute_group cpufreq_ajust_threshold_attribute_group = {
    .name = "cpufreq_ajust_threshold",
    .attrs = cpufreq_ajust_threshold,
};


STATIC ssize_t get_android_sleep_policy(struct device *dev, struct device_attribute *attr, char *buf)
{
    return snprintf(buf, PAGE_SIZE-1, "current_sleep_policy = %d\n", g_sleep_policy);
}


STATIC ssize_t set_android_sleep_policy(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int32        new_policy;

    if (g_powermgmt_switch)
    {
        /* 从Android上层读入睡眠策略
         * 0:从不       17分钟去关联
         * 1:仅限充电时 插USB充电时，始终保持关联,拔除USB时，按"从不"的策略来
         * 2:始终       始终保持关联
         * PS:
         * 充电或者选始终保持关联，需要将组播帧过滤关掉，同时配DTIM1
         */
        new_policy = buf[0];

        if (g_sleep_policy == new_policy)
        {
            HWIFI_INFO("already config sleep policy...");
        }
        else
        {
            HWIFI_INFO("new_policy(value = %d)\n", new_policy);
            g_sleep_policy = new_policy;
        }
    }

    return count;
}

STATIC DEVICE_ATTR(sleep_policy, S_IRUGO | S_IWUSR, get_android_sleep_policy, set_android_sleep_policy);
STATIC struct attribute *sleep_policy_entity[] = {
    &dev_attr_sleep_policy.attr,
    NULL
};
STATIC struct attribute_group g_sleep_policy_attribute_group = {
    .name = NULL,
    .attrs = sleep_policy_entity,
};



STATIC ssize_t get_watchdog_timeout(struct device *dev, struct device_attribute *attr, char *buf)
{
    return snprintf(buf, PAGE_SIZE-1, "current_watchdog_timeout = %d\n", g_watchdog_timeout);
}


STATIC ssize_t set_watchdog_timeout(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int32 param_cnt;
    int32 watchdog_timeout;

    param_cnt = sscanf(buf, "%d", &watchdog_timeout);
    if (param_cnt != 1)
    {
        printk(KERN_ERR"error param\n");

        return count;
    }
    printk(KERN_INFO"\n");

    if (watchdog_timeout > DEFAULT_WATCHDOG_TIMEOUT)
    {
        g_watchdog_timeout = DEFAULT_WATCHDOG_TIMEOUT;
    }
    else if (watchdog_timeout < MIN_WATCHDOG_TIMEOUT)
    {
        HWIFI_WARNING("watchdogtimeout value is too small, set to Default");
        g_watchdog_timeout = DEFAULT_WATCHDOG_TIMEOUT;
    }
    else
    {
        g_watchdog_timeout = watchdog_timeout;
    }

    return count;
}

STATIC DEVICE_ATTR(watchdog_timeout, S_IRUGO | S_IWUSR, get_watchdog_timeout, set_watchdog_timeout);
STATIC struct attribute *watchdog_timeout_entity[] = {
    &dev_attr_watchdog_timeout.attr,
    NULL
};
STATIC struct attribute_group g_watchdog_timeout_attribute_group = {
    .name = NULL,
    .attrs = watchdog_timeout_entity,
};



STATIC ssize_t get_flowctrl(struct device *dev, struct device_attribute *attr, char *buf)
{
    return snprintf(buf, PAGE_SIZE-1, "current state: %s, on-count: %u, off-count: %u, assemble-reset-count: %u\n",
                (hcc_get_dev_flowctrl() == FLOWCTRL_ON ? "ON" : "OFF"),
                g_hi110x_dev->hcc->dev_flowctrl_on_count,
                g_hi110x_dev->hcc->dev_flowctrl_off_count,
                g_hi110x_dev->hcc->dev_assemble_reset_count
                );

}


ssize_t set_flowctrl_on(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int flowctrl;
    
    int32 param_cnt;

    param_cnt = sscanf(buf, "%d", &flowctrl);
    if (param_cnt != 1)
    {
        printk(KERN_EMERG"error param\n");

        return count;
    }

    HWIFI_WARNING("user set flowectrl %s!", flowctrl?"on":"off");
    if(flowctrl)
    {
        hcc_dev_flowctrl_on(g_hi110x_dev->hcc, 1);
    }
    else
    {
        hcc_dev_flowctrl_off(g_hi110x_dev->hcc, HWIFI_HOST2DEV);
    }

    return count;
}

STATIC ssize_t get_flowctrl_enable(struct device *dev, struct device_attribute *attr, char *buf)
{
    ssize_t ret = 0;
    ret += snprintf(buf+ret, PAGE_SIZE-1, "This used to set whether force to enable flowctrl when flowctrl lock too long. \n" );
    ret += snprintf(buf+ret, PAGE_SIZE-1, "Now we %s force open flowctrl.", g_hi110x_dev->hcc->dev_flow_ctrl.force_ctrl_on ?"can":"can't");    
    return ret;
}

ssize_t set_flowctrl_enable(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int flowctrl;
    sscanf(buf, "%d", &flowctrl);
    HWIFI_INFO("user set force flowectrl %s!\n", flowctrl?"enable":"disable");
    g_hi110x_dev->hcc->dev_flow_ctrl.force_ctrl_on= !!flowctrl;
    return count;
}

STATIC DEVICE_ATTR(flowctrl, S_IRUGO | S_IWUSR, get_flowctrl, set_flowctrl_on);
STATIC DEVICE_ATTR(flowctrl_force, S_IRUGO | S_IWUSR, get_flowctrl_enable, set_flowctrl_enable);
STATIC struct attribute *flowctrl_entity[] = {
    &dev_attr_flowctrl.attr,
    &dev_attr_flowctrl_force.attr,  
    NULL
};

STATIC struct attribute_group g_flowctrl_attribute_group = {
    .name = NULL,
    .attrs = flowctrl_entity,
};



STATIC ssize_t get_packet_stats(struct device *dev, struct device_attribute *attr, char *buf)
{
    return snprintf(buf, PAGE_SIZE-1, "arp tx count %u, eapol tx count %u, dhcp tx count %u\n",
                g_hi110x_dev->hcc->tx_arp_count,
                g_hi110x_dev->hcc->tx_eapol_count,
                g_hi110x_dev->hcc->tx_dhcp_count
                );

}

STATIC DEVICE_ATTR(packet_stats, S_IRUGO, get_packet_stats, NULL);
STATIC struct attribute *packet_stats_entity[] = {
    &dev_attr_packet_stats.attr,
    NULL
};

STATIC struct attribute_group g_packet_stats_attribute_group = {
    .name = NULL,
    .attrs = packet_stats_entity,
};


STATIC ssize_t get_exception_enable(struct device *dev, struct device_attribute *attr, char *buf)
{
    return snprintf(buf, PAGE_SIZE-1, "current_status = %d\n", exception_enable);
}


STATIC ssize_t set_exception_enable(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int32 param_cnt;
    int32 level;

    param_cnt = sscanf(buf, "%d", &level);
    if (param_cnt != 1)
    {
        printk(KERN_ERR"error param\n");

        return count;
    }
    printk(KERN_INFO"\n");

    exception_enable = level;

    return count;
}


STATIC DEVICE_ATTR(exception_enable, S_IRUGO | S_IWUSR, get_exception_enable, set_exception_enable);
STATIC struct attribute *exception_enable_entity[] = {
    &dev_attr_exception_enable.attr,
    NULL
};
STATIC struct attribute_group g_exception_enable_attribute_group = {
    .name = NULL,
    .attrs = exception_enable_entity,
};

#ifdef HCC_DEBUG
STATIC ssize_t set_hcc_queues_count(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int i = 0;
    if(NULL == g_hi110x_dev)
    {
        printk(KERN_EMERG"g_hi110x_dev is null!\n");
        return count;
    }
    for(i = 0; i< HCC_QUEUE_COUNT; i++)
    {
        atomic_set(&g_hi110x_dev->hcc->hcc_queues[HCC_TX].queues[i].queue_count, 0);
        atomic_set(&g_hi110x_dev->hcc->hcc_queues[HCC_RX].queues[i].queue_count, 0);
        g_hi110x_dev->hcc->hcc_queues[HCC_TX].queues[i].drop_count = 0;
        g_hi110x_dev->hcc->hcc_queues[HCC_RX].queues[i].drop_count = 0;
    }
    atomic_set(&g_hi110x_dev->hcc->debug.hcc_tx_count, 0);
    atomic_set(&g_hi110x_dev->hcc->debug.hcc_single_count, 0);
    atomic_set(&g_hi110x_dev->hcc->debug.hcc_assem_count, 0);
    printk(KERN_EMERG"reset hcc queues done!\n");
    return count;
}
STATIC ssize_t get_hcc_queues_count(struct device *dev, struct device_attribute *attr, char *buf)
{
    int i,j;
    ssize_t ret = 0;
    int total_len = 0;
    struct hcc_handler   *hcc = NULL;
    if(NULL == g_hi110x_dev)
    {
        return snprintf(buf, PAGE_SIZE-1, "g_hi110x_dev is null!\n");
    }
    hcc = g_hi110x_dev->hcc;
    ret += snprintf(buf + ret, PAGE_SIZE-1, "hcc channel stat:\n");
    ret += snprintf(buf + ret, PAGE_SIZE-1, "hcc_tx_count \t[%d]\n",
                    atomic_read(&g_hi110x_dev->hcc->debug.hcc_tx_count));
    ret += snprintf(buf + ret, PAGE_SIZE-1, "hcc_single_count \t[%d]\n",
                    atomic_read(&g_hi110x_dev->hcc->debug.hcc_single_count));
    ret += snprintf(buf + ret, PAGE_SIZE-1, "hcc_assem_count \t[%d]\n",
                    atomic_read(&g_hi110x_dev->hcc->debug.hcc_assem_count));
    ret += snprintf(buf + ret, PAGE_SIZE-1, "\ntx queues count\n");
    for(i=0; i<HCC_QUEUE_COUNT; i++)
    {
         int len = atomic_read(&g_hi110x_dev->hcc->hcc_queues[HCC_TX].queues[i].queue_count);
         ret += snprintf(buf + ret, PAGE_SIZE-1, "%-30s  \tqlen[%u]\n",
                          hcc_get_queue_str(i),
                          len);
         total_len += len;
    }
    ret += snprintf(buf + ret, PAGE_SIZE-1, "tx queues total count[%d]\n", total_len);
    total_len = 0;
    ret += snprintf(buf + ret, PAGE_SIZE-1, "\nrx queues count\n");
    for(i=0; i<HCC_QUEUE_COUNT; i++)
    {
         int len = atomic_read(&g_hi110x_dev->hcc->hcc_queues[HCC_RX].queues[i].queue_count);
         ret += snprintf(buf + ret, PAGE_SIZE-1, "%-30s  \tqlen[%u]\n",
                          hcc_get_queue_str(i),
                          len);
         total_len += len;
    }
    ret += snprintf(buf + ret, PAGE_SIZE-1, "rx queues total count[%d]\n\n", total_len);
    for(j=0; j < HCC_DIR_COUNT; j++)
    {
        unsigned long long total_drop = 0;
        ret += snprintf(buf + ret, PAGE_SIZE-1, "\n%s queues drop count\n",
                                    hcc_get_dir_str(j));
        for(i=0; i<HCC_QUEUE_COUNT; i++)
        {
            if(hcc->hcc_queues[j].queues[i].drop_count)
            {
                total_drop += hcc->hcc_queues[j].queues[i].drop_count;
                ret += snprintf(buf + ret, PAGE_SIZE-1, "\n%-30s  \t: %llu\n",
                                hcc_get_queue_str(i),
                                hcc->hcc_queues[j].queues[i].drop_count);
            }
        }
        ret += snprintf(buf + ret, PAGE_SIZE-1, "\n%s total count:%llu\n",
                            hcc_get_dir_str(j), total_drop);
    }
    HWIFI_WARN_ON(ret > PAGE_SIZE);
    return ret;
}
STATIC DEVICE_ATTR(queues_stat, S_IRUGO | S_IWUSR, get_hcc_queues_count, set_hcc_queues_count);

STATIC ssize_t set_hcc_peak_stat(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    if(NULL == g_hi110x_dev)
    {
        printk(KERN_EMERG"g_hi110x_dev is null!\n");
        return count;
    }
    hcc_debug_peak_reset(g_hi110x_dev->hcc);
    printk(KERN_EMERG"reset hcc queues done!\n");
    return count;
}
STATIC ssize_t get_hcc_peak_stat(struct device *dev, struct device_attribute *attr, char *buf)
{
    ssize_t ret = 0;
    hwifi_union_log log={0};
    HWIFI_UNION_LOG_INIT(&log,KERN_EMERG);
    if(NULL == g_hi110x_dev)
    {
        return snprintf(buf, PAGE_SIZE-1, "g_hi110x_dev is null!\n");
    }
    ret += snprintf(buf + ret, PAGE_SIZE-1, "log had been moved to kernel kmsg\n");
    hcc_debug_peak_show(g_hi110x_dev->hcc, &log);
    return ret;
}
STATIC DEVICE_ATTR(peak_stat, S_IRUGO | S_IWUSR, get_hcc_peak_stat, set_hcc_peak_stat);
STATIC struct attribute *hcc_debug_entity[] = {
    &dev_attr_queues_stat.attr,
    &dev_attr_peak_stat.attr,
    NULL
};
STATIC struct attribute_group hcc_debug_attribute_group = {
    .name = "hcc_debug",
    .attrs = hcc_debug_entity,
};
#endif

/*hcc filter info*/
STATIC ssize_t set_hcc_stream_info(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int i = 0;
    if(NULL == g_hi110x_dev)
    {
        printk(KERN_EMERG"g_hi110x_dev is null!\n");
        return count;
    }

    for(i = 0; i < HCC_DIR_COUNT ; i++)
    {
        memset(g_hi110x_dev->hcc->hcc_queues[i].filter_info.tcp_info, 0, sizeof(wlan_perform_tcp_impl)*HCC_TCP_STREAM);
        g_hi110x_dev->hcc->hcc_queues[i].filter_info.ignored_count = 0;
    }

    printk(KERN_EMERG"reset hcc filter_info done!\n");
    return count;
}

STATIC ssize_t get_hcc_stream_info(struct device *dev, struct device_attribute *attr, char *buf)
{
    int i =0;
    ssize_t ret = 0;
    int stream = 0;
    unsigned long long drop_count;
    unsigned long long send_count;
    unsigned long long dup_ack_count;
    struct hcc_handler* hcc = NULL;
    if(NULL == g_hi110x_dev)
    {
        return snprintf(buf, PAGE_SIZE-1, "g_hi110x_dev is null!\n");
    }
    hcc = g_hi110x_dev->hcc;
    for(i = 0 ;i < HCC_DIR_COUNT; i++)
    {
        drop_count    = 0;
        send_count    = 0;
        dup_ack_count = 0;
        ret += snprintf(buf +ret, PAGE_SIZE-1, "%s:\n", hcc_get_dir_str(i));
        ret += snprintf(buf +ret, PAGE_SIZE-1, "%8s %8s %20s %30s %20s\n", "stream",
                                          "limit","drop count","duplicate ack count",
                                          "send count");
        for(stream=0; stream < HCC_TCP_STREAM; stream++)
        {
            drop_count      += hcc->hcc_queues[i].filter_info.tcp_info[stream].drop_count;
            dup_ack_count   += hcc->hcc_queues[i].filter_info.tcp_info[stream].dup_ack_count;
            send_count      += hcc->hcc_queues[i].filter_info.tcp_info[stream].send_count;
            ret += snprintf(buf+ret, PAGE_SIZE-1, "%8d %8d %20llu %30llu %20llu\n",stream,
                        hcc->hcc_queues[i].filter_info.ack_limit,
                        hcc->hcc_queues[i].filter_info.tcp_info[stream].drop_count,
                        hcc->hcc_queues[i].filter_info.tcp_info[stream].dup_ack_count,
                        hcc->hcc_queues[i].filter_info.tcp_info[stream].send_count);
        }
        ret += snprintf(buf+ret, PAGE_SIZE-1,"ignored count:%llu\n",hcc->hcc_queues[i].filter_info.ignored_count);
        ret += snprintf(buf+ret, PAGE_SIZE-1, "%8s %8s %20llu %30llu %20llu\n\n","total:","",
                        drop_count, dup_ack_count, send_count+hcc->hcc_queues[i].filter_info.ignored_count);
    }
    if(HWIFI_WARN(ret > PAGE_SIZE, "ret=%zd expect < %lu\n",ret, PAGE_SIZE))
    {
        HWIFI_ERROR("Fatal Error!! buf is too long!!");
    }
    return ret;
}

STATIC DEVICE_ATTR(stream_info, S_IRUGO | S_IWUSR, get_hcc_stream_info, set_hcc_stream_info);

STATIC ssize_t set_hcc_tcp_ack_limit(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int i = 0;
    int ack_limit[HCC_DIR_COUNT] = {0};
    struct hcc_handler* hcc = NULL;
    if(NULL == g_hi110x_dev)
    {
        printk(KERN_EMERG"g_hi110x_dev is null!\n");
        return count;
    }
    hcc = g_hi110x_dev->hcc;
    if(sscanf(buf, "%d %d", &ack_limit[HCC_TX], &ack_limit[HCC_RX]) != HCC_DIR_COUNT)
    {
        printk(KERN_EMERG"wrong format, echo '1 1' > ack_limit \n");
        return count;
    }
    for(i=0; i < HCC_DIR_COUNT; i++)
    {
        printk(KERN_EMERG"%s ack limit change from %20d to %20d ", hcc_get_dir_str(i),
                            hcc->hcc_queues[i].filter_info.ack_limit,ack_limit[i]);
        hcc->hcc_queues[i].filter_info.ack_limit = ack_limit[i];
    }

    return count;
}

STATIC ssize_t get_hcc_tcp_ack_limit(struct device *dev, struct device_attribute *attr, char *buf)
{
    int i =0;
    ssize_t ret = 0;
    struct hcc_handler* hcc = NULL;
    if(NULL == g_hi110x_dev)
    {
        return snprintf(buf, PAGE_SIZE-1, "g_hi110x_dev is null!\n");
    }
    hcc = g_hi110x_dev->hcc;
    ret += snprintf(buf+ret, PAGE_SIZE-1,
        "usage:\n   If you want to modify ack limit\n    echo '1(tx) 1(rx)' > /sys/hi110x/filter/ack_limit \n");
    for(i = 0; i < HCC_DIR_COUNT; i++)
    {
        ret += snprintf(buf+ret, PAGE_SIZE-1, "%s ack limit: %d\n",
               hcc_get_dir_str(i),hcc->hcc_queues[i].filter_info.ack_limit);
    }
    HWIFI_WARN(ret > PAGE_SIZE, "ret=%zd expect < %lu\n",ret, PAGE_SIZE);
    return ret;
}

STATIC DEVICE_ATTR(ack_limit, S_IRUGO | S_IWUSR, get_hcc_tcp_ack_limit, set_hcc_tcp_ack_limit);
STATIC struct attribute *hcc_filter[] = {
    &dev_attr_stream_info.attr,
    &dev_attr_ack_limit.attr,
    NULL
};
STATIC struct attribute_group hcc_filter_attribute_group = {
    .name = "filter",
    .attrs = hcc_filter,
};

STATIC ssize_t get_hcc_queues_status(struct device *dev, struct device_attribute *attr, char *buf)
{
    int i;
    ssize_t ret = 0;
    if(NULL == g_hi110x_dev)
    {
        return snprintf(buf, PAGE_SIZE-1, "g_hi110x_dev is null!\n");
    }
    hcc_transfer_lock(g_hi110x_dev->hcc);
    ret += snprintf(buf + ret, PAGE_SIZE-1, "rx & tx queues status:\n");
    for(i = 0;i < HCC_QUEUE_COUNT;i++)
    {
        ret += snprintf(buf + ret, PAGE_SIZE-1, "rx:%-30s \tqlen[%u]\n",
                    hcc_get_queue_str(i),
                    skb_queue_len(&g_hi110x_dev->hcc->hcc_queues[HCC_RX].queues[i].data_queue));
    }
    ret += snprintf(buf + ret, PAGE_SIZE-1, "rx:wait count:%d\n",
                     atomic_read(&g_hi110x_dev->hcc->hcc_queues[HCC_RX].wait_count));
    ret += snprintf(buf + ret, PAGE_SIZE-1, "rx:total count:%d\n",
                     atomic_read(&g_hi110x_dev->hcc->hcc_queues[HCC_RX].count));                     
    for(i = 0;i < HCC_QUEUE_COUNT;i++)
    {
        ret += snprintf(buf + ret, PAGE_SIZE-1, "tx:%-30s \tqlen[%u]\n",
                    hcc_get_queue_str(i),
                    skb_queue_len(&g_hi110x_dev->hcc->hcc_queues[HCC_TX].queues[i].data_queue));
    }
    ret += snprintf(buf + ret, PAGE_SIZE-1, "tx:wait count:%d\n",
                     atomic_read(&g_hi110x_dev->hcc->hcc_queues[HCC_TX].wait_count));
    ret += snprintf(buf + ret, PAGE_SIZE-1, "tx:total count:%d\n",
                     atomic_read(&g_hi110x_dev->hcc->hcc_queues[HCC_TX].count));                             
    hcc_transfer_unlock(g_hi110x_dev->hcc);
    HWIFI_WARN_ON(ret > PAGE_SIZE);
    return ret;
}
STATIC DEVICE_ATTR(queues_status, S_IRUGO, get_hcc_queues_status, NULL);
STATIC ssize_t get_hcc_queues_limit(struct device *dev, struct device_attribute *attr, char *buf)
{
    int i;
    ssize_t ret = 0;
    uint32 limit;
    
    if(NULL == g_hi110x_dev)
    {
        return snprintf(buf, PAGE_SIZE-1, "g_hi110x_dev is null!\n");
    }
    for(i=0; i<HCC_QUEUE_COUNT; i++)
    {
         limit = g_hi110x_dev->hcc->hcc_queues[HCC_TX].queues[i].queue_trans_limit;
        
        ret += snprintf(buf + ret, PAGE_SIZE-1, "tx:%-30s  \tlimit %u 0x%08X\n",
                          hcc_get_queue_str(i),
                          limit, limit);
    }
    for(i=0; i<HCC_QUEUE_COUNT; i++)
    {
         limit = g_hi110x_dev->hcc->hcc_queues[HCC_RX].queues[i].queue_trans_limit;
        
         ret += snprintf(buf + ret, PAGE_SIZE-1, "rx:%-30s  \tlimit %u 0x%08X\n",
                          hcc_get_queue_str(i),
                          limit, limit);
    }
    HWIFI_WARN_ON(ret > PAGE_SIZE);
    return ret;
}
STATIC ssize_t set_hcc_queues_limit(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    if(NULL == g_hi110x_dev)
    {
        printk(KERN_EMERG"g_hi110x_dev is null!\n");
        return count;
    }
    printk(KERN_EMERG"set limit to /sys/hi110x/hcc/limit_set/*!\n");
    return count;
}
STATIC DEVICE_ATTR(queues_limit, S_IRUGO | S_IWUSR, get_hcc_queues_limit, set_hcc_queues_limit);
STATIC ssize_t set_hcc_sched(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    uint32 input_count = 0;
    int ret = 0;
    struct hcc_handler* hcc = NULL;
    if(NULL == g_hi110x_dev)
    {
        printk(KERN_EMERG"g_hi110x_dev is null!\n");
        return count;
    }
    hcc = g_hi110x_dev->hcc;
    ret = sscanf(buf, "%u", &input_count);
    if(1 != ret)
    {
        printk(KERN_EMERG"wrong input argument format, just 1 input, but now : %d", ret);
        return count;
    }
    hcc_sched_transfer(hcc);
    return count;
}
STATIC DEVICE_ATTR(hcc_sched, S_IWUSR, NULL, set_hcc_sched);

STATIC ssize_t get_hcc_action_count(struct device *dev, struct device_attribute *attr, char *buf)
{
    int i,j;
    ssize_t ret = 0;
    if(NULL == g_hi110x_dev)
    {
        return snprintf(buf, PAGE_SIZE-1, "g_hi110x_dev is null!\n");
    }
    ret += snprintf(buf + ret , PAGE_SIZE-1,"dump hcc action info:\n");
    for(i = 0;i < HCC_DIR_COUNT;i++)
    {
        unsigned long long total_len = 0;
        ret += snprintf(buf + ret, PAGE_SIZE-1,"%s action count:\n", hcc_get_dir_str(i));
        for(j = 0;j < HCC_ACTION_COUNT;j++)
        {
            ret += snprintf(buf + ret, PAGE_SIZE-1,"%30s: \t %llu \n", hcc_get_action_str(j),
                    g_hi110x_dev->hcc->hcc_queues[i].hcc_action_count[j]);
            total_len+=g_hi110x_dev->hcc->hcc_queues[i].hcc_action_count[j];
        }
        ret += snprintf(buf + ret, PAGE_SIZE-1,"total len:%llu \n\n", total_len);
    }
    HWIFI_WARN_ON(ret > PAGE_SIZE);
    return ret;
}
STATIC ssize_t set_hcc_action_count(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int i;
    if(NULL == g_hi110x_dev)
    {
        printk(KERN_EMERG"g_hi110x_dev is null!\n");
        return count;
    }
    for(i = 0;i < HCC_DIR_COUNT;i++)
    {
        memset(g_hi110x_dev->hcc->hcc_queues[i].hcc_action_count, 0,
               HCC_ACTION_COUNT*sizeof(unsigned long long));
    }
    printk(KERN_EMERG"rest action info done.\n");
    return count;
}
STATIC DEVICE_ATTR(hcc_action_count, S_IRUGO|S_IWUSR, get_hcc_action_count, set_hcc_action_count);

STATIC ssize_t get_hcc_bt_opt_status(struct device *dev, struct device_attribute *attr, char *buf)
{
    int i = 0;
    ssize_t ret = 0;
    if(NULL == g_hi110x_dev)
    {
        return snprintf(buf, PAGE_SIZE-1, "g_hi110x_dev is null!\n");
    }

    ret += snprintf(buf + ret , PAGE_SIZE-1,"echo dir en/disable ms > bt_opt_stat \n");
    ret += snprintf(buf + ret , PAGE_SIZE-1,"echo tx 1 1500 > bt_opt_stat \n");
    ret += snprintf(buf + ret , PAGE_SIZE-1,"echo rx 1 1500 > bt_opt_stat \n");
    ret += snprintf(buf + ret , PAGE_SIZE-1,"bt opt status:\n");

    for(i = 0; i < HCC_DIR_COUNT; i++)
    {
        ret += snprintf(buf + ret , PAGE_SIZE-1,"%s\n", hcc_get_dir_str(i));
        ret += snprintf(buf + ret , PAGE_SIZE-1,"bt opt had %s ", 
        g_hi110x_dev->hcc->bt_opt.item[i].enable?"enabled":"disabed");
        if(g_hi110x_dev->hcc->bt_opt.item[i].enable)
            ret += snprintf(buf + ret , PAGE_SIZE-1,"%d ms, stat:%d", 
                jiffies_to_msecs(g_hi110x_dev->hcc->bt_opt.item[i].timeout), 
                g_hi110x_dev->hcc->bt_opt.stat);          
        ret += snprintf(buf + ret , PAGE_SIZE-1,"\n");                
    }
    
    HWIFI_WARN_ON(ret > PAGE_SIZE);
    return ret;
}

STATIC ssize_t set_hcc_bt_opt_timeout(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int ret;
    int enable;
    int i = 0;
    char buff[50]={0};
    unsigned int jiffies_i;
    if(NULL == g_hi110x_dev)
    {
        printk(KERN_EMERG"g_hi110x_dev is null!\n");
        return count;
    }
    ret = sscanf(buf, "%s %d %u", buff,&enable,  &jiffies_i);
    if(ret != 3)
    {
        printk(KERN_EMERG"unvaild param, should like echo tx 1 1500 >  bt_opt_stat\n");
        return count;
    }        

    if(!strcmp(buff, "tx") || !strcmp(buff, "TX"))
    {
        i = HCC_TX;
    }
    else if(!strcmp(buff, "rx") || !strcmp(buff, "RX"))
    {
        i = HCC_RX;
    }
    else
    {
        printk(KERN_EMERG"unvaild param, should like echo tx 1 1500 >  bt_opt_stat\n");
        return count;    
    }
    

    enable = !!enable;

    if(enable != g_hi110x_dev->hcc->bt_opt.item[i].enable)
    {
        g_hi110x_dev->hcc->bt_opt.item[i].enable = FALSE;
        del_timer_sync(&g_hi110x_dev->hcc->bt_opt.item[i].bt_opt_timer);
        g_hi110x_dev->hcc->bt_opt.item[i].enable = enable;
        printk(KERN_EMERG"had %s %s bt opt function.\n", enable?"enabled":"disabed",
                    hcc_get_dir_str(i));
    }
    
    printk(KERN_EMERG"change timeout from %u to %u\n", 
                jiffies_to_msecs(g_hi110x_dev->hcc->bt_opt.item[i].timeout),
                jiffies_i);
    g_hi110x_dev->hcc->bt_opt.item[i].timeout = msecs_to_jiffies(jiffies_i);
    
    return count;
}
STATIC DEVICE_ATTR(bt_opt_stat, S_IRUGO|S_IWUSR, get_hcc_bt_opt_status, set_hcc_bt_opt_timeout);


DECLARE_HCC_SYS_ENTRY(hcc_dyn_pri_enable);
DECLARE_HCC_SYS_ENTRY(hcc_dyn_pri_mask);
DECLARE_HCC_SYS_ENTRY(tcp_rx_ack_opt_enable);
DECLARE_HCC_SYS_ENTRY(tcp_tx_ack_opt_enable);
DECLARE_HCC_SYS_ENTRY(hcc_rx_opt_enable);
DECLARE_HCC_SYS_ENTRY(hcc_tx_assem_opt_enable);

#ifdef WMM_OPT_FOR_AUTH
DECLARE_HCC_SYS_ENTRY(wmm_ac_opt_enable);
#endif

STATIC struct attribute *hcc_entity[] = {
    &dev_attr_hcc_action_count.attr,
    &dev_attr_queues_limit.attr,
    &dev_attr_queues_status.attr,
    &dev_attr_hcc_sched.attr,
    &dev_attr_hcc_dyn_pri_enable.attr,
    &dev_attr_hcc_dyn_pri_mask.attr,
    &dev_attr_tcp_rx_ack_opt_enable.attr,
    &dev_attr_tcp_tx_ack_opt_enable.attr,
    &dev_attr_hcc_rx_opt_enable.attr,
    &dev_attr_hcc_tx_assem_opt_enable.attr,
#ifdef WMM_OPT_FOR_AUTH
    &dev_attr_wmm_ac_opt_enable.attr,
    &dev_attr_wmm_drop_interval.attr,
    &dev_attr_wmm_drop_cnt_per_time.attr,
    &dev_attr_wmm_que_len_for_drop.attr,
#endif
    &dev_attr_bt_opt_stat.attr,
    NULL
};
STATIC struct attribute_group hcc_queues_attribute_group = {
    .name = NULL,
    .attrs = hcc_entity,
};

STATIC ssize_t set_hcc_flow_ctrl_wl(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int i;
    ssize_t ret = 0;
    int lo_wl, hi_wl;
    char buff_t[100] = {0};
    struct hcc_handler* hcc = NULL;
    if(NULL == g_hi110x_dev)
    {
        printk(KERN_EMERG"g_hi110x_dev is null!\n");
        return count;
    }
    hcc = g_hi110x_dev->hcc;
    ret = sscanf(buf, "%100s %d %d", buff_t,  &lo_wl, &hi_wl);
    if(3!= ret)
    {
        printk(KERN_EMERG"wrong input argument format, want 3 input, but now : %zd\n", ret);
        return count;
    }    
    printk(KERN_EMERG"prepare SET queue %s lo_waterline:%d, hi_waterline:%d\n", buff_t, lo_wl, hi_wl);
    for(i  = 0; i < HCC_QUEUE_COUNT;i++)
    {
        char* name = hcc_get_queue_str(i);
       if( !strcmp(name, buff_t))
       {
            printk(KERN_EMERG"find the queue:%s\n",name);
            break;
       }
    }
    if(i == HCC_QUEUE_COUNT)
    {
        printk(KERN_EMERG"unkown queue:%s\n",buff_t);
        return count;
    }
    hcc->hcc_queues[HCC_TX].queues[i].flow_ctrl.lo_waterline = lo_wl;
    hcc->hcc_queues[HCC_TX].queues[i].flow_ctrl.hi_waterline = hi_wl;    
    return count;
}

STATIC ssize_t get_hcc_flow_ctrl_wl(struct device *dev, struct device_attribute *attr, char *buf)
{
    int i;
    ssize_t ret = 0;
    struct hcc_handler* hcc = NULL;
    if(NULL == g_hi110x_dev)
    {
        return snprintf(buf, PAGE_SIZE-1, "g_hi110x_dev is null!\n");
    }
    hcc = g_hi110x_dev->hcc;
    ret += snprintf(buf + ret, PAGE_SIZE-1,"Hcc tx flow ctrl status(just show the queues under flow ctrl):\n");
    ret += snprintf(buf + ret, PAGE_SIZE-1,"%-30s %10s %10s\n", "QUEUE", "LO_WLINE","HI_WLINE");
    for(i = 0; i < HCC_QUEUE_COUNT; i++)
    {
        if(!hcc->hcc_queues[HCC_TX].queues[i].flow_ctrl.enable)
            continue;
        ret += snprintf(buf + ret, PAGE_SIZE-1,"%-30s %10d %10d\n",
        hcc_get_queue_str(i),
        hcc->hcc_queues[HCC_TX].queues[i].flow_ctrl.lo_waterline,
        hcc->hcc_queues[HCC_TX].queues[i].flow_ctrl.hi_waterline);    
    }
    ret += snprintf(buf + ret, PAGE_SIZE-1,"How to change the lo hi waterline? Sample:\n");
    ret += snprintf(buf + ret, PAGE_SIZE-1,"    echo 'DATA_UDP_VI_QUEUE 10 20' > waterline\n");
    ret += snprintf(buf + ret, PAGE_SIZE-1,"      DATA_UDP_VI_QUEUE is queue name , 10 is low waterline, 20 is high waterline\n");
    HWIFI_WARN_ON(ret > PAGE_SIZE);
    return ret;
}

//STATIC struct device_attribute dev_attr_flow_ctrl_enable = __ATTR(enable, S_IRUGO | S_IWGRP, get_hcc_flow_ctrl_enable, set_hcc_flow_ctrl_enable);
STATIC struct device_attribute dev_attr_flow_ctrl_waterline = __ATTR(waterline, S_IRUGO | S_IWUSR, get_hcc_flow_ctrl_wl, set_hcc_flow_ctrl_wl);
STATIC struct attribute *flow_ctrl_entity[] = {
//  &dev_attr_flow_ctrl_enable.attr,
    &dev_attr_flow_ctrl_waterline.attr,
    NULL
};
STATIC struct attribute_group flow_ctrl_attribute_group = {
    .name = "flow_ctrl",
    .attrs = flow_ctrl_entity,
};

STATIC ssize_t hcc_net_stream_get_stat(struct device *dev, struct device_attribute *attr, char *buf)
{
    int i;
    ssize_t ret = 0;
    struct hcc_handler* hcc = NULL;
    if(NULL == g_hi110x_dev)
    {
        return snprintf(buf, PAGE_SIZE-1, "g_hi110x_dev is null!\n");
    }
    hcc = g_hi110x_dev->hcc;
    ret += snprintf(buf + ret, PAGE_SIZE-1,"Hcc net stream stat\n");
    ret += snprintf(buf + ret, PAGE_SIZE-1,"%-30s %10s %30s %10s %10s %15s %20s \n", "stream", "ignored","stat","sm_pkts","total", "conn_limit", "discn_limit(%)");

    for(i = 0; i < STREAM_COUNT; i++)
    {
        ret += snprintf(buf + ret, PAGE_SIZE-1,"%-30s %10s %30s %10lu %10lu %15lu %20lu\n",
            hcc->net_stream.stream[i].name,
            hcc->net_stream.stream[i].ignored ?"N/A":"",
            hcc_get_net_stream_sm_str(hcc->net_stream.stream[i].sm_stat),
            hcc->net_stream.stream[i].sm_pkt_count,
            hcc->net_stream.stream[i].sm_tx_pack,
            hcc->net_stream.stream[i].connecting_pkts_limit,
            hcc->net_stream.stream[i].disconn_percent_limit);    
    }

    ret +=snprintf(buf + ret, PAGE_SIZE-1, "aggree stat:%s\n", 
                                                hcc->hi110x_dev->cfg->aggre_info.aggr_start ?"start":"stop");
    
    HWIFI_WARN_ON(ret > PAGE_SIZE);
    return ret;
}

STATIC struct device_attribute dev_attr_net_stream_stat = __ATTR(stat, S_IRUGO, hcc_net_stream_get_stat, NULL);

STATIC ssize_t hcc_net_stream_set_period(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int i;
    int ret = 0;
    unsigned long conncet_period, disconn_period;
    char buff_t[100] = {0};
    struct hcc_handler* hcc = NULL;
    if(NULL == g_hi110x_dev)
    {
        printk(KERN_EMERG"g_hi110x_dev is null!\n");
        return count;
    }
    hcc = g_hi110x_dev->hcc;
    ret = sscanf(buf, "%50s %lu %lu", buff_t,  &conncet_period, &disconn_period);
    if(3!= ret)
    {
        printk(KERN_EMERG"wrong input argument format, want 3 input, but now : %d\n", ret);
        return count;
    }    
    
    i = hcc_find_stream_by_name(hcc, buff_t);
    if(i == STREAM_COUNT)
    {
        printk(KERN_EMERG"unkown stream:%s\n",buff_t);
        return count;
    }
    hcc->net_stream.stream[i].connecting_period = msecs_to_jiffies(conncet_period) ?:10;
    hcc->net_stream.stream[i].disconn_period = msecs_to_jiffies(disconn_period) ?:10;
    return count;
}

STATIC ssize_t hcc_net_stream_get_period(struct device *dev, struct device_attribute *attr, char *buf)
{
    int i;
    ssize_t ret = 0;
    struct hcc_handler* hcc = NULL;
    if(NULL == g_hi110x_dev)
    {
        return snprintf(buf, PAGE_SIZE-1, "g_hi110x_dev is null!\n");
    }
    hcc = g_hi110x_dev->hcc;
    ret += snprintf(buf + ret, PAGE_SIZE-1,"Hcc net stream connecting  & disconnct scan period(millisecond):\n");
    ret += snprintf(buf + ret, PAGE_SIZE-1,"%-30s %20s %20s\n", "stream","connect_period","disconn_period");
    for(i = 0; i < STREAM_COUNT; i++)
    {
        if(TRUE == hcc->net_stream.stream[i].ignored)
            continue;
        ret += snprintf(buf + ret, PAGE_SIZE-1,"%-30s %20u %20u\n",
            hcc->net_stream.stream[i].name,
            jiffies_to_msecs(hcc->net_stream.stream[i].connecting_period),
            jiffies_to_msecs(hcc->net_stream.stream[i].disconn_period));    
    }
    ret += snprintf(buf + ret, PAGE_SIZE-1,"Usage: How to change the scan period? Sample:\n");
    ret += snprintf(buf + ret, PAGE_SIZE-1,"    echo 'udp_rtp 1000 10000' > period\n");
    ret += snprintf(buf + ret, PAGE_SIZE-1,"      udp_rtp  is stream name , 1000(ms) is connect scan period, 10000 is disconnect scan period\n");
    HWIFI_WARN_ON(ret > PAGE_SIZE);
    return ret;
}
STATIC struct device_attribute dev_attr_net_stream_period = __ATTR(period, S_IRUGO|S_IWUSR, hcc_net_stream_get_period, hcc_net_stream_set_period);

STATIC ssize_t hcc_net_stream_set_limit(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int i;
    int ret = 0;
    unsigned long connecting_pkts_limit, disconn_percent_limit;
    char buff_t[100] = {0};
    struct hcc_handler* hcc = NULL;
    if(NULL == g_hi110x_dev)
    {
        printk(KERN_EMERG"g_hi110x_dev is null!\n");
        return count;
    }
    hcc = g_hi110x_dev->hcc;
    ret = sscanf(buf, "%50s %lu %lu", buff_t,  &connecting_pkts_limit, &disconn_percent_limit);
    if(3!= ret)
    {
        printk(KERN_EMERG"wrong input argument format, want 3 input, but now : %d\n", ret);
        return count;
    }    
    
    i = hcc_find_stream_by_name(hcc, buff_t);
    if(i == STREAM_COUNT)
    {
        printk(KERN_EMERG"unkown stream:%s\n",buff_t);
        return count;
    }
    hcc->net_stream.stream[i].connecting_pkts_limit = connecting_pkts_limit;
    hcc->net_stream.stream[i].disconn_percent_limit = disconn_percent_limit;
    return count;
}

STATIC ssize_t hcc_net_stream_get_limit(struct device *dev, struct device_attribute *attr, char *buf)
{
    int i;
    ssize_t ret = 0;
    struct hcc_handler* hcc = NULL;
    if(NULL == g_hi110x_dev)
    {
        return snprintf(buf, PAGE_SIZE-1, "g_hi110x_dev is null!\n");
    }
    hcc = g_hi110x_dev->hcc;
    ret += snprintf(buf + ret, PAGE_SIZE-1,"Hcc net stream connecting  & disconnct pkts limit:\n");
    ret += snprintf(buf + ret, PAGE_SIZE-1,"%-30s %20s %20s\n", "stream","connect_limit","disconn_per(%)");
    for(i = 0; i < STREAM_COUNT; i++)
    {
        if(TRUE == hcc->net_stream.stream[i].ignored)
            continue;
        ret += snprintf(buf + ret, PAGE_SIZE-1,"%-30s %20lu %20lu%%\n",
            hcc->net_stream.stream[i].name,
            hcc->net_stream.stream[i].connecting_pkts_limit,
            hcc->net_stream.stream[i].disconn_percent_limit);    
    }
    ret += snprintf(buf + ret, PAGE_SIZE-1,"Usage: How to change the connect & disconnect limit? Sample:\n");
    ret += snprintf(buf + ret, PAGE_SIZE-1,"    echo 'udp_rtp 100 1' > limit\n");
    ret += snprintf(buf + ret, PAGE_SIZE-1,"      udp_rtp  is stream name , 100(pkts) is if over 100 packages when connected, "
                                                                            "1 is if disconnect when low than 1%%\n");
    HWIFI_WARN_ON(ret > PAGE_SIZE);
    return ret;
}
STATIC struct device_attribute dev_attr_net_stream_limit     = __ATTR(limit, S_IRUGO|S_IWUSR, hcc_net_stream_get_limit, hcc_net_stream_set_limit);

STATIC ssize_t hcc_net_stream_set_ignore(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int i;
    int ret = 0;
    int ignored;
    char buff_t[100] = {0};
    struct hcc_handler* hcc = NULL;
    if(NULL == g_hi110x_dev)
    {
        printk(KERN_EMERG"g_hi110x_dev is null!\n");
        return count;
    }
    hcc = g_hi110x_dev->hcc;
    ret = sscanf(buf, "%50s %d", buff_t,  &ignored);
    if(3!= ret)
    {
        printk(KERN_EMERG"wrong input argument format, want 3 input, but now : %d\n", ret);
        return count;
    }    
    
    i = hcc_find_stream_by_name(hcc, buff_t);
    if(i == STREAM_COUNT)
    {
        printk(KERN_EMERG"unkown stream:%s\n",buff_t);
        return count;
    }
    hcc->net_stream.stream[i].ignored = !!ignored;
    if(ignored)
    {
        del_timer_sync(&hcc->net_stream.stream[i].timer);
        hcc->net_stream.stream[i].sm_stat = HNET_STREAM_DISCONN;
    }
    return count;
}

STATIC ssize_t hcc_net_stream_get_ignore(struct device *dev, struct device_attribute *attr, char *buf)
{
    int i;
    ssize_t ret = 0;
    struct hcc_handler* hcc = NULL;
    if(NULL == g_hi110x_dev)
    {
        return snprintf(buf, PAGE_SIZE-1, "g_hi110x_dev is null!\n");
    }
    hcc = g_hi110x_dev->hcc;
    ret += snprintf(buf + ret, PAGE_SIZE-1,"Hcc net stream ignored flag:\n");
    ret += snprintf(buf + ret, PAGE_SIZE-1,"%-30s %10s\n", "stream","ignored");
    for(i = 0; i < STREAM_COUNT; i++)
    {
        ret += snprintf(buf + ret, PAGE_SIZE-1,"%-30s %10d\n",
            hcc->net_stream.stream[i].name,
            hcc->net_stream.stream[i].ignored);    
    }
    ret += snprintf(buf + ret, PAGE_SIZE-1,"Usage: How to change the net stream ignored flag?1:means stream ignored, 0 means not Sample:\n");
    ret += snprintf(buf + ret, PAGE_SIZE-1,"    echo 'udp_rtp 1' > ignore\n");
    ret += snprintf(buf + ret, PAGE_SIZE-1,"      udp_rtp  is stream name , 100(pkts) is if over 100 packages when connected, "
                                                                            "1 is if disconnect when low than 1%%\n");
    HWIFI_WARN_ON(ret > PAGE_SIZE);
    return ret;
}
STATIC struct device_attribute dev_attr_net_stream_ignored     = __ATTR(ignore, S_IRUGO|S_IWUSR, hcc_net_stream_get_ignore, hcc_net_stream_set_ignore);
STATIC struct attribute *net_stream_stat_entity[] = {
    &dev_attr_net_stream_stat.attr,
    &dev_attr_net_stream_period.attr,
    &dev_attr_net_stream_limit.attr,
    &dev_attr_net_stream_ignored.attr,
    NULL
};
STATIC struct attribute_group net_stream_attribute_group = {
    .name = "net_stream",
    .attrs = net_stream_stat_entity,
};

DECLARE_HCC_LIMIT(tx_ctrl_limit,        HCC_TX, CTRL_QUEUE);
DECLARE_HCC_LIMIT(tx_hi_data_limit,     HCC_TX, DATA_HI_QUEUE);
DECLARE_HCC_LIMIT(tx_tcp_data_limit,    HCC_TX, DATA_TCP_DATA_QUEUE);
DECLARE_HCC_LIMIT(tx_tcp_ack_limit,     HCC_TX, DATA_TCP_ACK_QUEUE);
DECLARE_HCC_LIMIT(tx_lo_data_limit,     HCC_TX, DATA_LO_QUEUE);
DECLARE_HCC_LIMIT(rx_ctrl_limit,        HCC_RX, CTRL_QUEUE);
DECLARE_HCC_LIMIT(rx_hi_data_limit,     HCC_RX, DATA_HI_QUEUE);
DECLARE_HCC_LIMIT(rx_tcp_data_limit,    HCC_RX, DATA_TCP_DATA_QUEUE);
DECLARE_HCC_LIMIT(rx_tcp_ack_limit,     HCC_RX, DATA_TCP_ACK_QUEUE);
DECLARE_HCC_LIMIT(rx_lo_data_limit,     HCC_RX, DATA_LO_QUEUE);

DECLARE_HCC_LIMIT(tx_udp_be_data_limit,     HCC_TX, DATA_UDP_BE_QUEUE);
DECLARE_HCC_LIMIT(tx_udp_bk_data_limit,     HCC_TX, DATA_UDP_BK_QUEUE);
DECLARE_HCC_LIMIT(tx_udp_vi_data_limit,     HCC_TX, DATA_UDP_VI_QUEUE);
DECLARE_HCC_LIMIT(tx_udp_vo_data_limit,     HCC_TX, DATA_UDP_VO_QUEUE);
DECLARE_HCC_LIMIT(rx_udp_be_data_limit,     HCC_RX, DATA_UDP_BE_QUEUE);
DECLARE_HCC_LIMIT(rx_udp_bk_data_limit,     HCC_RX, DATA_UDP_BK_QUEUE);
DECLARE_HCC_LIMIT(rx_udp_vi_data_limit,     HCC_RX, DATA_UDP_VI_QUEUE);
DECLARE_HCC_LIMIT(rx_udp_vo_data_limit,     HCC_RX, DATA_UDP_VO_QUEUE);


DECLARE_HCC_QUOTIETY(down_quotiety);
DECLARE_HCC_QUOTIETY(up_quotiety);
STATIC struct attribute *limit_set_entity[] = {
    &dev_attr_tx_ctrl_limit.attr,
    &dev_attr_tx_hi_data_limit.attr,
    &dev_attr_tx_tcp_data_limit.attr,
    &dev_attr_tx_tcp_ack_limit.attr,
    &dev_attr_tx_lo_data_limit.attr,

    &dev_attr_tx_udp_be_data_limit.attr,
    &dev_attr_tx_udp_bk_data_limit.attr,
    &dev_attr_tx_udp_vi_data_limit.attr,
    &dev_attr_tx_udp_vo_data_limit.attr,

    &dev_attr_rx_ctrl_limit.attr,
    &dev_attr_rx_hi_data_limit.attr,
    &dev_attr_rx_tcp_data_limit.attr,
    &dev_attr_rx_tcp_ack_limit.attr,
    &dev_attr_rx_lo_data_limit.attr,

    &dev_attr_rx_udp_be_data_limit.attr,
    &dev_attr_rx_udp_bk_data_limit.attr,
    &dev_attr_rx_udp_vi_data_limit.attr,
    &dev_attr_rx_udp_vo_data_limit.attr,

    &dev_attr_tcp_down_quotiety.attr,
    &dev_attr_tcp_up_quotiety.attr,
    NULL
};
STATIC struct attribute_group hcc_limit_set_attribute_group = {
    .name = "limit_set",
    .attrs = limit_set_entity,
};

void hwifi_union_log_register(hwifi_union_log* log, void* data)
{
    HWIFI_BUG_ON(!log);
    HWIFI_UNION_LOG_INIT(log,"");
    log->data = data;
    list_add_tail(&log->list, &hi110x_log_head);
    HWIFI_INFO("Log module %s added![%pF]", log->module,(void*)_RET_IP_);
}

/*
 * Prototype    : userctr_loglevel_init
 * Description  : init the interface
 * Input        : void
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/26
 *     Author       : z00209041
 *     Modification : Created function
 *
 */
int32 userctrl_init(void)
{
    int32   ret = SUCC;

    g_sysfs_hi110x = kobject_create_and_add("hi110x", NULL);
    if (NULL == g_sysfs_hi110x)
    {
        goto failed_g_sysfs_hi110x_obj;
    }

#ifdef HCC_DEBUG
    ret = sysfs_create_group(g_sysfs_hi110x, &hcc_debug_attribute_group);
    if(ret)
    {
        goto failed_hcc_debug_group;
    }
#endif
    ret = sysfs_create_group(g_sysfs_hi110x, &hcc_filter_attribute_group);
    if(ret)
    {
        goto failed_hcc_filter_group;
    }
    
    hcc_obj = kobject_create_and_add("hcc", g_sysfs_hi110x);
    if(!hcc_obj)
    {
        ret = -EFAIL;
        goto failed_hcc_obj;
    }

    ret = sysfs_create_group(hcc_obj, &hcc_queues_attribute_group);
    if(ret)
    {
        goto failed_hcc_queues_group;
    }
    
    ret = sysfs_create_group(hcc_obj, &hcc_limit_set_attribute_group);
    if(ret)
    {
        goto failed_hcc_limit_set_group;
    }

    ret = sysfs_create_group(g_sysfs_hi110x, &g_loglevel_attribute_group);
    if (SUCC != ret)
    {
        goto failed_g_loglevel_group;
    }

    ret = sysfs_create_group(g_sysfs_hi110x, &g_powermgmt_switch_attribute_group);
    if (SUCC != ret)
    {
        goto failed_g_powermgmt_switch_group;
    }

    ret = sysfs_create_group(g_sysfs_hi110x, &g_cur_cpufreq_attribute_group);
    if (SUCC != ret)
    {
        goto failed_g_cur_cpufreq_group;
    }

    ret = sysfs_create_group(g_sysfs_hi110x, &cpufreq_ajust_threshold_attribute_group);
    if (SUCC != ret)
    {
        goto failed_cpufreq_ajust_threshold_attribute_group;
    }

    ret = sysfs_create_group(g_sysfs_hi110x, &perform_mode_attribute_group);
    if (SUCC != ret)
    {
        goto failed_perform_mode_group;
    }

    ret = sysfs_create_group(g_sysfs_hi110x, &g_flowctrl_attribute_group);
    if (SUCC != ret)
    {
        goto failed_g_flowctrl_group;
    }

    ret = sysfs_create_group(g_sysfs_hi110x, &g_packet_stats_attribute_group);
    if (SUCC != ret)
    {
        goto failed_g_packet_stats_group;
    }

    ret = sysfs_create_group(g_sysfs_hi110x, &g_sleep_policy_attribute_group);
    if (SUCC != ret)
    {
        goto failed_g_sleep_policy_group;
    }

    ret = sysfs_create_group(g_sysfs_hi110x, &g_watchdog_timeout_attribute_group);
    if (SUCC != ret)
    {
        goto failed_g_watchdog_timeout_group;
    }

    ret = sysfs_create_group(g_sysfs_hi110x, &g_exception_enable_attribute_group);
    if (SUCC != ret)
    {
        goto failed_g_exception_enable_group;
    }

    ret = sysfs_create_group(g_sysfs_hi110x, &g_coex_level_attribute_group);

    if (SUCC != ret)
    {
        goto failed_g_coex_level_attribute_group;
    }

    atomic_notifier_chain_register(&panic_notifier_list,
                       &hwifi_panic_notifier);

    ret = sysfs_create_group(hcc_obj, &flow_ctrl_attribute_group);
    if(ret)
    {
        goto failed_flow_ctrl_attribute_group;        
    }

    ret = sysfs_create_group(hcc_obj, &net_stream_attribute_group);
    if(ret)
    {
        goto failed_net_stream_attribute_group;
    }

    return SUCC;
failed_net_stream_attribute_group:
    sysfs_remove_group(hcc_obj, &flow_ctrl_attribute_group);
failed_flow_ctrl_attribute_group:
    atomic_notifier_chain_unregister(&panic_notifier_list,
                                    &hwifi_panic_notifier);
failed_g_coex_level_attribute_group:
    sysfs_remove_group(g_sysfs_hi110x, &g_exception_enable_attribute_group);
failed_g_exception_enable_group:
    sysfs_remove_group(g_sysfs_hi110x, &g_watchdog_timeout_attribute_group);
failed_g_watchdog_timeout_group:
    sysfs_remove_group(g_sysfs_hi110x, &g_sleep_policy_attribute_group);
failed_g_sleep_policy_group:
    sysfs_remove_group(g_sysfs_hi110x, &g_packet_stats_attribute_group);
failed_g_packet_stats_group:
    sysfs_remove_group(g_sysfs_hi110x, &g_flowctrl_attribute_group);
failed_g_flowctrl_group:
    sysfs_remove_group(g_sysfs_hi110x, &perform_mode_attribute_group);
failed_perform_mode_group:
    sysfs_remove_group(g_sysfs_hi110x, &cpufreq_ajust_threshold_attribute_group);
failed_cpufreq_ajust_threshold_attribute_group:
    sysfs_remove_group(g_sysfs_hi110x, &g_cur_cpufreq_attribute_group);
failed_g_cur_cpufreq_group:
    sysfs_remove_group(g_sysfs_hi110x, &g_powermgmt_switch_attribute_group);
failed_g_powermgmt_switch_group:
    sysfs_remove_group(g_sysfs_hi110x, &g_loglevel_attribute_group);
failed_g_loglevel_group:
    sysfs_remove_group(hcc_obj, &hcc_limit_set_attribute_group);
failed_hcc_limit_set_group:
    sysfs_remove_group(g_sysfs_hi110x, &hcc_queues_attribute_group);
failed_hcc_queues_group:
    kobject_put(hcc_obj);
failed_hcc_obj:
    sysfs_remove_group(g_sysfs_hi110x, &hcc_filter_attribute_group);
failed_hcc_filter_group:
#ifdef HCC_DEBUG
    sysfs_remove_group(g_sysfs_hi110x, &hcc_debug_attribute_group);
#endif
failed_hcc_debug_group:
    kobject_put(g_sysfs_hi110x);
failed_g_sysfs_hi110x_obj:
    HWIFI_ERROR("userctrl init erro=%d", ret);
    return ret;
}


void userctrl_exit(void)
{
    if(hcc_obj)
    {
        sysfs_remove_group(hcc_obj, &net_stream_attribute_group);
        sysfs_remove_group(hcc_obj, &flow_ctrl_attribute_group);
    }
    atomic_notifier_chain_unregister(&panic_notifier_list,
                                    &hwifi_panic_notifier);
    if (g_sysfs_hi110x)
    {
        sysfs_remove_group(g_sysfs_hi110x, &g_loglevel_attribute_group);
        sysfs_remove_group(g_sysfs_hi110x, &g_flowctrl_attribute_group);
        sysfs_remove_group(g_sysfs_hi110x, &g_watchdog_timeout_attribute_group);
        sysfs_remove_group(g_sysfs_hi110x, &g_powermgmt_switch_attribute_group);
        sysfs_remove_group(g_sysfs_hi110x, &g_packet_stats_attribute_group);
        sysfs_remove_group(g_sysfs_hi110x, &g_exception_enable_attribute_group);
        sysfs_remove_group(g_sysfs_hi110x, &hcc_filter_attribute_group);

        sysfs_remove_group(g_sysfs_hi110x, &cpufreq_ajust_threshold_attribute_group);
        sysfs_remove_group(g_sysfs_hi110x, &perform_mode_attribute_group);
        sysfs_remove_group(g_sysfs_hi110x, &g_sleep_policy_attribute_group);

#ifdef HCC_DEBUG
        sysfs_remove_group(g_sysfs_hi110x, &hcc_debug_attribute_group);
#endif
        if(hcc_obj)
        {
            sysfs_remove_group(hcc_obj, &hcc_limit_set_attribute_group);
            sysfs_remove_group(hcc_obj, &hcc_queues_attribute_group);
            kobject_put(hcc_obj);
            hcc_obj = NULL;
        }
        kobject_put(g_sysfs_hi110x);
        g_sysfs_hi110x = NULL;
    }
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
