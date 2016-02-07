
#include "usb_vendor.h"

#if USB_IS_SUPPORT_PMU
#include "bsp_pmu.h"
#endif

#if USB_IS_SUPPORT_OM_DUMP
#include "bsp_dump.h"
#endif

#if USB_IS_SUPPORT_NV
#include "drv_nvim.h"
#include "drv_nv_id.h"
#endif

#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/module.h>
#include <linux/export.h>
#include <linux/delay.h>
#include <linux/fcntl.h>
#include <linux/namei.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/syscalls.h>
#include <linux/wakelock.h>



#if USB_IS_SUPPORT_VSHELL
static int usb_vendor_console_init(void)
{
	return bsp_usb_console_init();
}
#else /* !USB_IS_SUPPORT_VSHELL */
static int usb_vendor_console_init(void)
{
	return 0;
}
#endif /* USB_IS_SUPPORT_VSHELL */


#if USB_IS_SUPPORT_DYN_INIT
static int usb_vendor_init(void)
{
	return usb_balong_init();
}

static void usb_vendor_exit(void)
{
	return usb_balong_exit();
}
#else /* !USB_IS_SUPPORT_DYN_INIT */
static int usb_vendor_init(void)
{
	return 0;
}

static void usb_vendor_exit(void)
{
	return;
}
#endif /* USB_IS_SUPPORT_DYN_INIT */


/*
 * usb adapter for charger
 */
typedef struct usb_notifer_ctx {
	int charger_type;
	int usb_status;
	int usb_old_status;
	int usb_start_inboot;
	char last_cdev_name[USB_CDEV_NAME_MAX];
	unsigned stat_usb_insert;
	unsigned stat_usb_insert_proc;
	unsigned stat_usb_insert_proc_end;
	unsigned stat_usb_enum_done;
	unsigned stat_usb_enum_done_proc;
	unsigned stat_usb_enum_done_proc_end;
	unsigned stat_usb_remove;
	unsigned stat_usb_remove_proc;
	unsigned stat_usb_remove_proc_end;
	unsigned stat_usb_disable;
	unsigned stat_usb_disable_proc;
	unsigned stat_usb_disable_proc_end;
	unsigned stat_usb_no_need_notify;
	unsigned stat_reg_pmu_cb_fail;
	unsigned stat_wait_cdev_created;

	struct workqueue_struct *usb_notify_wq;
	struct delayed_work usb_notify_wk;
} usb_notifer_ctx_t;

static BLOCKING_NOTIFIER_HEAD(usb_balong_notifier_list);
static usb_notifer_ctx_t g_usb_notifier;

/*
 * usb enum done management
 */

static usb_enum_stat_t g_usb_devices_enum_stat[USB_FD_DEVICE_MAX];
static unsigned g_usb_dev_enum_done_num;
static unsigned g_usb_dev_setup_num;
//static spinlock_t g_usb_dev_enum_lock;


/*
 * usb enum done management implement
 */
void bsp_usb_init_enum_stat(void)
{
	//spin_lock_init(&g_usb_dev_enum_lock);
	g_usb_dev_enum_done_num = 0;
	g_usb_dev_setup_num = 0;
	memset(g_usb_devices_enum_stat, 0, sizeof(g_usb_devices_enum_stat));
}

void bsp_usb_add_setup_dev_fdname(unsigned intf_id, char* fd_name)
{
	if (g_usb_dev_setup_num >= USB_FD_DEVICE_MAX) {
		printk("%s error, setup_num:%d, USB_FD_DEVICE_MAX:%d\n",
		       __FUNCTION__, g_usb_dev_setup_num, USB_FD_DEVICE_MAX);
		return;
	}
	g_usb_devices_enum_stat[g_usb_dev_setup_num].usb_intf_id = intf_id;
	g_usb_devices_enum_stat[g_usb_dev_setup_num].fd_name = fd_name;
	g_usb_dev_setup_num++;
}

int bsp_usb_is_all_enum(void)
{
	return (g_usb_dev_setup_num != 0 && g_usb_dev_setup_num == g_usb_dev_enum_done_num);
}

int bsp_usb_is_all_disable(void)
{
	return (g_usb_dev_setup_num != 0 && 0 == g_usb_dev_enum_done_num);
}

void bsp_usb_set_enum_stat(unsigned intf_id, int enum_stat)
{
	int i;
	usb_enum_stat_t *find_dev = NULL;

	if (enum_stat) {
		/* if all dev is already enum, do nothing */
		if (bsp_usb_is_all_enum()) {
			return;
		}

		for (i = 0; i < g_usb_dev_setup_num; i++) {
			if (g_usb_devices_enum_stat[i].usb_intf_id == intf_id) {
				find_dev = &g_usb_devices_enum_stat[i];
			}
		}
		if (find_dev) {
			/* if the dev is already enum, do nothing */
			if (find_dev->is_enum) {
				return;
			}
			find_dev->is_enum = enum_stat;
			g_usb_dev_enum_done_num++;

			/* after change stat, if all dev enum done, notify callback */
			if (bsp_usb_is_all_enum()) {
				bsp_usb_status_change(USB_BALONG_ENUM_DONE);
			}
		}
	} else {
		if (bsp_usb_is_all_disable()) {
			return;
		}

		for (i = 0; i < g_usb_dev_setup_num; i++) {
			if (g_usb_devices_enum_stat[i].usb_intf_id == intf_id) {
				find_dev = &g_usb_devices_enum_stat[i];
			}
		}
		if (find_dev) {
			/* if the dev is already disable, do nothing */
			if (!find_dev->is_enum) {
				return;
			}
			find_dev->is_enum = enum_stat;

			/* g_usb_dev_enum_done_num is always > 0,
			 * we protect it in check bsp_usb_is_all_disable
			 */
			if (g_usb_dev_enum_done_num > 0) {
				g_usb_dev_enum_done_num--;
			}

			/* if the version is not support pmu detect
			 * and all the device is disable, we assume that the usb is remove,
			 * so notify disable callback, tell the other modules
			 * else, we use the pmu remove detect.
			 */
			if (!bsp_usb_is_support_pmu_detect ()
			    && bsp_usb_is_all_disable()) {
				bsp_usb_status_change(USB_BALONG_DEVICE_DISABLE);
			}
		}
	}

	return;
}

void bsp_usb_enum_info_dump(void)
{
	int i;

	printk("balong usb is enum done:%d\n", bsp_usb_is_all_enum());
	printk("setup_num:%d, enum_done_num:%d\n",
	       g_usb_dev_setup_num, g_usb_dev_enum_done_num);
	printk("device enum info details:\n\n");
	for (i = 0; i < g_usb_dev_setup_num; i++) {
		printk("enum dev:%d\n", i);
		printk("usb_intf_id:%d\n", g_usb_devices_enum_stat[i].usb_intf_id);
		printk("is_enum:%d\n", g_usb_devices_enum_stat[i].is_enum);
		printk("fd_name:%s\n\n", g_usb_devices_enum_stat[i].fd_name);
	}
}


/*
 * usb charger adapter implement
 */
void bsp_usb_status_change(int status)
{
	unsigned long timeout = 0;

	if (USB_BALONG_ENUM_DONE == status) {
		g_usb_notifier.stat_usb_enum_done++;
	} else if (USB_BALONG_DEVICE_INSERT == status) {
		g_usb_notifier.stat_usb_insert++;
	} else if (USB_BALONG_DEVICE_REMOVE == status) {
		g_usb_notifier.stat_usb_remove++;
	} else if (USB_BALONG_DEVICE_DISABLE == status) {
		g_usb_notifier.stat_usb_disable++;
	} else {
		printk("%s: error status:%d\n", __FUNCTION__, status);
	}

	g_usb_notifier.usb_status = status;

	queue_delayed_work(g_usb_notifier.usb_notify_wq,
	                   &g_usb_notifier.usb_notify_wk, timeout);
}
EXPORT_SYMBOL_GPL(bsp_usb_status_change);


#if USB_IS_SUPPORT_WAKELOCK
static struct wake_lock g_dwc_wakelock;
static inline void usb_vendor_init_wakelock(void)
{
	wake_lock_init(&g_dwc_wakelock, WAKE_LOCK_SUSPEND, "dwc3-wakelock");
}

static inline void usb_vendor_wake_lock(void)
{
	wake_lock(&g_dwc_wakelock);
}

static inline void usb_vendor_wake_unlock(void)
{
	wake_unlock(&g_dwc_wakelock);
}
#else
static inline void usb_vendor_init_wakelock(void)
{
	return;
}

static inline void usb_vendor_wake_lock(void)
{
	return;
}

static inline void usb_vendor_wake_unlock(void)
{
	return;
}
#endif


#if USB_IS_SUPPORT_PMU
static void bsp_usb_pmu_insert_detect(void* ctx)
{
	/* need to check whether usb is connected, as the PMU could
	 * report the false event during rapid inserting & pluging
	 */
	if (!bsp_pmu_usb_state_get()) {
		printk(KERN_WARNING "false insert event from PMU detected\n");
		return;
	}
	usb_vendor_wake_lock();

	printk(KERN_DEBUG "DWC3 USB: pmu insert event received\n");
	if (bsp_usb_is_support_pmu_detect()) {
		/* usb gadget driver will catch the insert event if USB_PMU_DETECT not defined */
		bsp_usb_status_change(USB_BALONG_DEVICE_INSERT);
	}
}

static void bsp_usb_pmu_remove_detect(void* ctx)
{
	/* need to check whether usb is connected, as the PMU could
	 * report the false event during rapid inserting & pluging
	 */
	if (bsp_pmu_usb_state_get()) {
		printk(KERN_WARNING "false remove event from PMU detected");
		return;
	}

	printk(KERN_DEBUG "DWC3 USB: pmu remove event received\n");
	if (bsp_usb_is_support_pmu_detect()) {
		/* usb gadget driver will catch the remove event if USB_PMU_DETECT not defined */
		bsp_usb_status_change(USB_BALONG_DEVICE_REMOVE);
	}
}

static int bsp_usb_pmu_detect_init(void)
{
	int ret = 0;

	/* reg usb insert/remove detect callback */
	if (bsp_pmu_irq_callback_register(PMU_INT_USB_IN,
	                                  bsp_usb_pmu_insert_detect, &g_usb_notifier)) {
		g_usb_notifier.stat_reg_pmu_cb_fail++;
	}
	if (bsp_pmu_irq_callback_register(PMU_INT_USB_OUT,
	                                  bsp_usb_pmu_remove_detect, &g_usb_notifier)) {
		g_usb_notifier.stat_reg_pmu_cb_fail++;
	}

	/* check whether the usb is connected in boot time */
	if (bsp_pmu_usb_state_get()) {
		printk("usb is connect in boot\n");
		usb_vendor_wake_lock();
		ret = 1;
	} else {
		printk("usb is not connect in boot\n");
		ret = 0;
	}

	if (!bsp_usb_is_support_pmu_detect()) {
		printk("usb is always init in boot\n");
		ret = 1;
	}

	return ret;
}
#else
static int bsp_usb_pmu_detect_init(void)
{
	return 0;
}
#endif

static void bsp_usb_insert_process(void)
{
	g_usb_notifier.stat_usb_insert_proc++;

	blocking_notifier_call_chain(&usb_balong_notifier_list,
	                             USB_BALONG_DEVICE_INSERT, (void*)&g_usb_notifier.charger_type);

	/* init the usb driver */
	usb_vendor_init();

	blocking_notifier_call_chain(&usb_balong_notifier_list,
	                             USB_BALONG_CHARGER_IDEN, (void*)&g_usb_notifier.charger_type);

	g_usb_notifier.stat_usb_insert_proc_end++;
	return;
}

void bsp_usb_set_last_cdev_name(char* dev_name)
{
	if (dev_name) {
		strncpy(g_usb_notifier.last_cdev_name, dev_name, USB_CDEV_NAME_MAX - 1);
		g_usb_notifier.last_cdev_name[USB_CDEV_NAME_MAX - 1] = '\0';
	}
}

void bsp_usb_clear_last_cdev_name(void)
{
	g_usb_notifier.last_cdev_name[0] = 0;
}

static void bsp_usb_wait_cdev_created(void)
{
	mm_segment_t old_fs;

	/* if there are no cdev, do nothing */
	if (strlen(g_usb_notifier.last_cdev_name) == 0) {
		return;
	}
	old_fs = get_fs();
	set_fs(KERNEL_DS);
	while (sys_faccessat(AT_FDCWD, g_usb_notifier.last_cdev_name, O_RDWR)) {
		g_usb_notifier.stat_wait_cdev_created++;
		msleep(100);
	}
	set_fs(old_fs);
}

static void bsp_usb_enum_done_process(void)
{
	printk("balong usb enum done.\n");
	g_usb_notifier.stat_usb_enum_done_proc++;

	/*
	 * wait for /dev/xxx created by udev
	 */
	bsp_usb_wait_cdev_created();

	/* notify kernel notifier */
	blocking_notifier_call_chain(&usb_balong_notifier_list,
	                             USB_BALONG_ENUM_DONE, (void*)&g_usb_notifier.charger_type);

	g_usb_notifier.stat_usb_enum_done_proc_end++;
}

static void bsp_usb_remove_device_process(void)
{
	printk("balong usb remove.\n");
	bsp_usb_clear_last_cdev_name();
	g_usb_notifier.stat_usb_remove_proc++;

	/* notify kernel notifier,
	 * we must call notifier list before disable callback,
	 * there are something need to do before user
	 */
	blocking_notifier_call_chain(&usb_balong_notifier_list,
	                             USB_BALONG_DEVICE_REMOVE, (void*)&g_usb_notifier.charger_type);

	/* exit the usb driver */
	usb_vendor_exit();

	g_usb_notifier.charger_type = USB_CHARGER_TYPE_INVALID;
	g_usb_notifier.stat_usb_remove_proc_end++;

	usb_vendor_wake_unlock();

	return;
}

static void bsp_usb_disable_device_process(void)
{
	printk("balong usb disable.\n");
	bsp_usb_clear_last_cdev_name();
	g_usb_notifier.stat_usb_disable_proc++;

	/* notify kernel notifier */
	blocking_notifier_call_chain(&usb_balong_notifier_list,
	                             USB_BALONG_DEVICE_DISABLE, (void*)&g_usb_notifier.charger_type);

	g_usb_notifier.charger_type = USB_CHARGER_TYPE_INVALID;
	g_usb_notifier.stat_usb_disable_proc_end++;

	usb_vendor_wake_unlock();

	return;
}


/**
 * usb_register_notify - register a notifier callback whenever a usb change happens
 * @nb: pointer to the notifier block for the callback events.
 *
 * These changes are either USB devices or busses being added or removed.
 */
void bsp_usb_register_notify(struct notifier_block *nb)
{
	blocking_notifier_chain_register(&usb_balong_notifier_list, nb);
}
EXPORT_SYMBOL_GPL(bsp_usb_register_notify);

/**
 * usb_unregister_notify - unregister a notifier callback
 * @nb: pointer to the notifier block for the callback events.
 *
 * usb_register_notify() must have been previously called for this function
 * to work properly.
 */
void bsp_usb_unregister_notify(struct notifier_block *nb)
{
	blocking_notifier_chain_unregister(&usb_balong_notifier_list, nb);
}
EXPORT_SYMBOL_GPL(bsp_usb_unregister_notify);

int bsp_usb_is_support_charger(void)
{
#if defined(CONFIG_USB_CHARGER_DETECT)
	return 1;
#else
	return 0;
#endif
}

int bsp_usb_is_support_pmu_detect(void)
{
#if defined(CONFIG_USB_PMU_DETECT)
	return 1;
#else
	return 0;
#endif
}


void bsp_usb_set_charger_type(int type)
{
	g_usb_notifier.charger_type = type;
	return;
}

int bsp_usb_get_charger_type(void)
{
	return g_usb_notifier.charger_type;
}

void bsp_usb_notifier_dump(void)
{
	if (USB_CHARGER_TYPE_HUAWEI == g_usb_notifier.charger_type) {
		printk("balong usb CHARGER_TYPE: HuaWei     :) \n");
	} else if (USB_CHARGER_TYPE_NOT_HUAWEI  == g_usb_notifier.charger_type) {
		printk("balong usb CHARGER_TYPE: Not HuaWei :( \n");
	} else {
		printk("balong usb CHARGER_TYPE: Invalid    +_+ \n");
	}
	printk("usb_start_inboot:           %d\n", g_usb_notifier.usb_start_inboot);
	printk("stat_usb_insert:            %d\n", g_usb_notifier.stat_usb_insert);
	printk("stat_usb_insert_proc:       %d\n", g_usb_notifier.stat_usb_insert_proc);
	printk("stat_usb_insert_proc_end:   %d\n", g_usb_notifier.stat_usb_insert_proc_end);
	printk("stat_usb_enum_done:         %d\n", g_usb_notifier.stat_usb_enum_done);
	printk("stat_usb_enum_done_proc:    %d\n", g_usb_notifier.stat_usb_enum_done_proc);
	printk("stat_usb_enum_done_proc_end:%d\n", g_usb_notifier.stat_usb_enum_done_proc_end);
	printk("stat_usb_remove:            %d\n", g_usb_notifier.stat_usb_remove);
	printk("stat_usb_remove_proc:       %d\n", g_usb_notifier.stat_usb_remove_proc);
	printk("stat_usb_remove_proc_end:   %d\n", g_usb_notifier.stat_usb_remove_proc_end);
	printk("stat_usb_disable:           %d\n", g_usb_notifier.stat_usb_disable);
	printk("stat_usb_disable_proc:      %d\n", g_usb_notifier.stat_usb_disable_proc);
	printk("stat_usb_disable_proc_end:  %d\n", g_usb_notifier.stat_usb_disable_proc_end);
	printk("usb_status:                 %d\n", g_usb_notifier.usb_status);
	printk("usb_old_status:             %d\n", g_usb_notifier.usb_old_status);
	printk("stat_wait_cdev_created:     %d\n", g_usb_notifier.stat_wait_cdev_created);
	printk("last_cdev_name:             %s\n", g_usb_notifier.last_cdev_name);
	printk("stat_reg_pmu_cb_fail:       %d\n", g_usb_notifier.stat_reg_pmu_cb_fail);
	printk("stat_usb_no_need_notify:    %d\n", g_usb_notifier.stat_usb_no_need_notify);
}

int usb_notifier_mem_dump(char* buffer, unsigned int buf_size)
{
	unsigned int need_size = sizeof(g_usb_notifier);

	if (need_size > buf_size) {
		return -1;
	}
	memcpy(buffer, &g_usb_notifier, need_size);
	return (int)need_size;
}

static void usb_notify_handler(struct work_struct *work)
{
	int cur_status = g_usb_notifier.usb_status;

	if (g_usb_notifier.usb_old_status == cur_status) {
		g_usb_notifier.stat_usb_no_need_notify++;
		return;
	}

	switch (cur_status) {
		case USB_BALONG_DEVICE_INSERT:
			bsp_usb_insert_process();
			break;
		case USB_BALONG_ENUM_DONE:
			bsp_usb_enum_done_process();
			break;
		case USB_BALONG_DEVICE_REMOVE:
			bsp_usb_remove_device_process();
			break;
		case USB_BALONG_DEVICE_DISABLE:
			bsp_usb_disable_device_process();
			break;
		default:
			printk("%s, invalid status:%d\n",
			       __FUNCTION__, cur_status);
			return;
	}
	g_usb_notifier.usb_old_status = cur_status;
	return;
}

static int bsp_usb_notifier_init(void)
{
	/* init local ctx resource */
	g_usb_notifier.charger_type = USB_CHARGER_TYPE_INVALID;
	g_usb_notifier.usb_notify_wq = create_singlethread_workqueue("usb_notify");
	if (!g_usb_notifier.usb_notify_wq) {
		printk("%s: create_singlethread_workqueue fail\n", __FUNCTION__);
		return -ENOMEM;
	}
	INIT_DELAYED_WORK(&g_usb_notifier.usb_notify_wk, (void *)usb_notify_handler);
	return 0;
}

#if USB_IS_SUPPORT_NV
/*
 * usb adapter for NV
 */

/* NV structure */
typedef struct {
	char wwan_flag;
	char reserved1;
	char reserved2;
	char reserved3;
	char reserved4;
	char reserved5;
	char reserved6;
	char reserved7;
} win7_feature_t;


typedef struct {
	/* usb nv structure ... */
	win7_feature_t win7_feature;
	unsigned int   shell_lock;

	/* stat counter */
	unsigned int stat_nv_read_fail;
} usb_nv_t;

/* USB NV ID */
#define USB_WIN7_FEATURE_NV                 NV_ID_DRV_WIN7_FEATURE
#define USB_ITEM_AT_SHELL_OPEN_FLAG         NV_ID_DRV_AT_SHELL_OPNE

static usb_nv_t g_usb_nv;

static void bsp_usb_nv_default_init(void)
{
	memset(&g_usb_nv, 0, sizeof(usb_nv_t));
}

int bsp_usb_is_support_wwan(void)
{
	return (int)(!!g_usb_nv.win7_feature.wwan_flag);
}

int bsp_usb_is_support_shell(void)
{
	return (int)(!g_usb_nv.shell_lock);
}

void bsp_usb_nv_dump(void)
{
	printk("usb read nv fail count: %d\n", g_usb_nv.stat_nv_read_fail);
	printk("is support wwan:        %d\n", g_usb_nv.win7_feature.wwan_flag);
	printk("is shell lock:          %d\n", g_usb_nv.shell_lock);
	return;
}

void bsp_usb_nv_init(void)
{
	/* init the default value */
	bsp_usb_nv_default_init();

	/*
	 * read needed usb nv items from nv
	 * ignore the nv_read return value,
	 * if fail, use the default value
	 */
	if (NVM_Read(USB_WIN7_FEATURE_NV,
	             &g_usb_nv.win7_feature, sizeof(g_usb_nv.win7_feature))) {
		g_usb_nv.stat_nv_read_fail++;
	}
	if (NVM_Read(USB_ITEM_AT_SHELL_OPEN_FLAG,
	             &g_usb_nv.shell_lock, sizeof(g_usb_nv.shell_lock))) {
		g_usb_nv.stat_nv_read_fail++;
	}
}
#else
int bsp_usb_is_support_wwan(void)
{
	return 0;
}

int bsp_usb_is_support_shell(void)
{
	return 0;
}

void bsp_usb_nv_init(void)
{
	return;
}
#endif /* USB_IS_SUPPORT_NV */


#if USB_IS_SUPPORT_OM_DUMP
/*
 * for usb om
 */
extern int perf_caller_mem_dump(char* buffer, unsigned int buf_size);
extern int acm_cdev_mem_dump(char* buffer, unsigned int buf_size);
extern int acm_serial_mem_dump(char* buffer, unsigned int buf_size);
extern int ncm_eth_mem_dump(char* buffer, unsigned int buf_size);


typedef int(*usb_adp_dump_func_t)(char* buffer, unsigned int buf_size);

struct usb_dump_func_info {
	char* mod_name;
	usb_adp_dump_func_t dump_func;
};

struct usb_dump_info_ctx {
	char* dump_buffer;
	char* cur_pos;
	unsigned int buf_size;
	unsigned int left;
};


/* mod_name must be 4 bytes, we just copy 4 bytes to dump buffer */
struct usb_dump_func_info usb_dump_func_tb[] = {
	{"perf", perf_caller_mem_dump},
	{"ntfy", usb_notifier_mem_dump},
	{"acmc", acm_cdev_mem_dump},
	{"acms", acm_serial_mem_dump},
	{"ncm_", ncm_eth_mem_dump},

	{"last", NULL}
};

#define USB_ADP_DUMP_FUNC_NUM \
    (sizeof(usb_dump_func_tb) / sizeof(struct usb_dump_func_info) - 1)

#define USB_ADP_GET_4BYTE_ALIGN(size) (((size) + 3) & (~0x3))

struct usb_dump_info_ctx usb_dump_ctx = {0};


void bsp_usb_adp_dump_hook(void)
{
	int i;
	int ret;
	char* cur_pos = usb_dump_ctx.dump_buffer;
	unsigned int left = usb_dump_ctx.buf_size;

	/* do nothing, if om init fail */
	if (0 == left || NULL == cur_pos)
		return;

	for (i = 0; i < USB_ADP_DUMP_FUNC_NUM; i++) {
		if (left <= 4) {
			break;
		}
		/* mark the module name, just copy 4 bytes to dump buffer */
		memcpy(cur_pos, usb_dump_func_tb[i].mod_name, 4);
		cur_pos += 4;
		left -= 4;

		if (usb_dump_func_tb[i].dump_func) {
			ret = usb_dump_func_tb[i].dump_func(cur_pos, left);
			/* may be no room left */
			if (ret < 0) {
				break;
			}

			/* different module recorder addr must be 4bytes align */
			ret = USB_ADP_GET_4BYTE_ALIGN(ret);
			cur_pos += ret;
			left -= ret;
		}
		/* NULL func is the last */
		else {
			break;
		}
	}

	usb_dump_ctx.cur_pos = cur_pos;
	usb_dump_ctx.left = left;
	return;
}

void bsp_usb_om_dump_init(void)
{
	/* reg the dump callback to om */
	if (bsp_dump_register_hook(DUMP_SAVE_MOD_USB,
	                           (dump_save_hook)bsp_usb_adp_dump_hook)) {
		goto err_ret;
	}

	if (bsp_dump_get_buffer(DUMP_SAVE_MOD_USB,
	                        &usb_dump_ctx.dump_buffer, &usb_dump_ctx.buf_size)) {
		goto err_ret;
	}

	return;

err_ret:
	usb_dump_ctx.dump_buffer = NULL;
	usb_dump_ctx.buf_size = 0;
	return;
}

void bsp_usb_om_dump_info(void)
{
	printk("buf_size:               %d\n", usb_dump_ctx.buf_size);
	printk("left:                   %d\n", usb_dump_ctx.left);
	printk("dump_buffer:            0x%x\n", (unsigned)usb_dump_ctx.dump_buffer);
	printk("cur_pos:                0x%x\n", (unsigned)usb_dump_ctx.cur_pos);
	return;
}
#else /* USB_IS_SUPPORT_OM_DUMP */
void bsp_usb_om_dump_init(void)
{
	return;
}
#endif /* USB_IS_SUPPORT_OM_DUMP */


/* we don't need exit for adapter module */
int __init bsp_usb_vendor_init(void)
{
	int ret = 0;
	int need_init = 0;

	ret = bsp_usb_notifier_init();
	if (ret) {
		return ret;
	}

	usb_vendor_init_wakelock();

	/* init usb needed nv */
	bsp_usb_nv_init();

	/* init om dump for usb */
	bsp_usb_om_dump_init();

	/* init pmu detect */
	need_init = bsp_usb_pmu_detect_init();

	/* if usb is connect, init the usb core */
	if (need_init) {
		g_usb_notifier.usb_start_inboot = 1;
		bsp_usb_status_change(USB_BALONG_DEVICE_INSERT);
	}

	/* usb console init */
	usb_vendor_console_init();

	return ret;
}
module_init(bsp_usb_vendor_init);
