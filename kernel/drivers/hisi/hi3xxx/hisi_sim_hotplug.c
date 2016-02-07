/*
 * hisi_sim_hotplug.c - SIM HOTPLUG driver
 *
 * Copyright (C) 2012 huawei Ltd.
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License. See the file "COPYING" in the main directory of this
 * archive for more details.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/module.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/clk.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/mutex.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <linux/err.h>
#include <linux/of_irq.h>
#include <linux/of_gpio.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/mfd/hi3xxx_hi6421v300.h>
#include "hisi_sim_hotplug.h"
#include <linux/wakelock.h>
#include <linux/huawei/rdr.h>

// DET PIN normal close or normal open
enum det_pin_set {
    NORMAL_CLOSE    = 0,
    NORMAL_OPEN     = 1,
    NORMAL_BUTT     = 2,
};

enum card_tray {
    SINGLE_CARD_IN_ONE_TRAY = 0,
    TWO_CARDS_IN_ONE_TRAY   = 1,
    CARD_TRAY_BUTT          = 2,
};

struct sim_hotplug_state_info {
	u32 sim_msg_input_value;
	u32 sim_msg_input_flag;
	u32 sim_msg_output_value;
	u32 sim_msg_output_flag;
};

static const char *work_name[] =
{
	"sim0_debounce_check",
	"sim1_debounce_check"
};
struct hisi_sim_hotplug_info {
	void __iomem *iomg_base;
	int	det_irq;
	int	det_gpio;
	struct workqueue_struct *sim_hotplug_det_wq;
	struct work_struct sim_hotplug_det_wk1;
	struct pinctrl *pctrl;
	struct pinctrl_state *sim_default;
	struct pinctrl_state *sim_idle;
	u8 sim_pluged;
	struct sim_hotplug_state_info sim_info;
	u32 sim_id;
	struct mutex sim_hotplug_lock;
	struct wake_lock  sim_hotplug_wklock;
	u8 set_gpio; // 1 indicates on, 0 indicates off, 2 indicates no gpio set
	u32 send_msg_to_lpm3;
    u32 det_direction; // 1 indicats open by default(high level), 0 indicats closed by default
	int old_det_gpio_state;
	u32 card_tray;
	u32 hpd_trigger;
	u32 debounce_settings;
	u32 debounce_wait_time;
	int debounce_checking;
	struct workqueue_struct *sim_debounce_delay_wq;
	struct delayed_work sim_debounce_delay_work;
};

enum sim_id {
	SIM0_ID = 0,
	SIM1_ID,
};

typedef struct {
	u32 time;
	u8 sim_no;
	u8 sim_status;
	u8 hpd_level;
	u8 det_level;
	u8 trace;
	u8 sim_mux[3];
} sim_log;

typedef struct {
	int sim_state;
	char* sim_state_str;
} sim_state_tbl;

static const sim_state_tbl s_sim_state_tbl[] = {
	{SIM_CARD_OUT,  "SIM_CARD_OUT"},
	{SIM_CARD_IN,   "SIM_CARD_IN"},
};

#define SIM_HOTPLUG_STATE_UNKNOWN   (0)
#define STR_SIM_CARD_IN             "1"
#define STR_CLEAR_SIM_CARD_STATE    "0"

struct kobject *sim_kobj;
static int sim0_state = SIM_HOTPLUG_STATE_UNKNOWN;
static int sim1_state = SIM_HOTPLUG_STATE_UNKNOWN;

/*line -emacro(750,*)*/
/*lint -e750*/
#define GPIO103_IOMG_ADDR	0x128
#define GPIO104_IOMG_ADDR	0x12c
#define GPIO105_IOMG_ADDR	0x130
#define GPIO106_IOMG_ADDR	0x134
#define GPIO107_IOMG_ADDR	0x138
#define GPIO108_IOMG_ADDR	0x13c
#define NO_NOTIFY		0
#define MAX_SIM_HOTPLUG_LOG	50
#define LOG_SIMHOTPLUG_BUF_LEN	600

#define HI6421V300_PMU_IRQ3                   (0X123) /* PMU IRQ3 */
#define HI6421V300_PMU_IRQ_MASK3              (0X105) /* PMU IRQ_MASK_3 */
#define HI6421V300_PMU_SIM_CTRL1              (0x0FE) /* PMU SIM_CTRL1 */
#define HI6421V300_PMU_SIM_CTRL2              (0x0FF) /* PMU SIM_CTRL2 */
#define HI6421V300_PMU_LDO11_CTRL             (0X08A) /* PMU LDO11_CTRL */
#define HI6421V300_PMU_LDO12_CTRL             (0X08B) /* PMU LDO12_CTRL */
#define HI6421V300_PMU_STATUS1                (0X002) /* PMU STATUS1 */

#define PMU_IRQ3          (0X33)
#define IRQ_MASK3         (0XED)
#define SIM_CTRL1         (0X12)
#define SIM_CTRL2         (0X27)
#define LDO11_CTRL        (0X21)
#define LDO12_CTRL        (0X21)
/*lint +e750*/
/*line +emacro(750,*)*/

#define READ_PMU_SIM(addr) readl_relaxed(hisi_pmurtc_base + (addr << 2))
#define WRITE_PMU_SIM(value,addr) writel_relaxed(value, hisi_pmurtc_base + (addr << 2))
#define SIM_DEBOUNCE_CHECK_WAIT_TIME   (300)   //ms

sim_log *g_log_org;
u8 g_sim_log_idx = 0;
void __iomem *hisi_pmurtc_base;

void sim_pmu_hpd_read(void)
{
    unsigned long pmu_irq3       = 0;
    unsigned long pmu_irq_mask3  = 0;
    unsigned long pmu_sim_ctrl1  = 0;
    unsigned long pmu_sim_ctrl2  = 0;
    unsigned long pmu_ldo11_ctrl = 0;
    unsigned long pmu_ldo12_ctrl = 0;
    unsigned long pmu_status1    = 0;

    pmu_irq3 = READ_PMU_SIM(HI6421V300_PMU_IRQ3);
    pmu_irq_mask3 = READ_PMU_SIM(HI6421V300_PMU_IRQ_MASK3);
    pmu_sim_ctrl1 = READ_PMU_SIM(HI6421V300_PMU_SIM_CTRL1);
    pmu_sim_ctrl2 = READ_PMU_SIM(HI6421V300_PMU_SIM_CTRL2);
    pmu_ldo11_ctrl = READ_PMU_SIM(HI6421V300_PMU_LDO11_CTRL);
    pmu_ldo12_ctrl = READ_PMU_SIM(HI6421V300_PMU_LDO12_CTRL);
    pmu_status1 = READ_PMU_SIM(HI6421V300_PMU_STATUS1);

    pr_info("%s: start\n", __func__);
    pr_info("%s: sim_pmu_status1:0X%lx\n", __func__, pmu_status1);
    pr_info("%s: sim_pmu_irq3:0X%lx pmu_irq_mask3:0X%lx\n", __func__, pmu_irq3, pmu_irq_mask3);
    pr_info("%s: sim_pmu_ctrl1:0X%lx pmu_ctrl2:0X%lx\n",__func__, pmu_sim_ctrl1, pmu_sim_ctrl2);
    pr_info("%s: sim_pmu_ldo11_ctrl:0X%lx pmu_ldo12_ctrl:0X%lx\n", __func__, pmu_ldo11_ctrl, pmu_ldo12_ctrl);
}

void sim_pmu_hpd_write(struct hisi_sim_hotplug_info *info)
{
    int irq_mask3 = 0;

    pr_info("%s: start\n", __func__);

    if (NULL == info) {
        pr_info("%s: end error, info is null!!! \n", __func__);
        return;
    }

    if (0xFF == info->hpd_trigger || 0 == info->hpd_trigger) {
        pr_info("%s: end error, hpd_trigger:0x%x \n", __func__, info->hpd_trigger);
        return;
    }

    irq_mask3 = (0xFF & (~info->hpd_trigger));
    pr_info("%s: irq_mask3:0x%x \n", __func__, irq_mask3);

    // clear interrupt
    WRITE_PMU_SIM(PMU_IRQ3, HI6421V300_PMU_IRQ3);
    // set interrupt mask register(IRQ_MASK3)
    WRITE_PMU_SIM(irq_mask3, HI6421V300_PMU_IRQ_MASK3);
    // tirgger for the hpd of pmu
    WRITE_PMU_SIM(info->hpd_trigger, HI6421V300_PMU_SIM_CTRL1);
    // debounce setting
    WRITE_PMU_SIM(info->debounce_settings, HI6421V300_PMU_SIM_CTRL2);
    // enable PMU, 1.8v by default
    WRITE_PMU_SIM(LDO11_CTRL, HI6421V300_PMU_LDO11_CTRL);
    WRITE_PMU_SIM(LDO12_CTRL, HI6421V300_PMU_LDO12_CTRL);
    pr_info("%s: end\n", __func__);
}

int sim_pmu_hpd_init(void)
{
    int ret = -1;
    struct device_node *node;

    node = of_find_compatible_node(NULL, NULL, "hisilicon,hi6421-pmurtc");
	if (!node) {
		pr_err("sim_pmu_hpd_init: of_find_compatible_node failed!\n");
		goto err;
	}

	hisi_pmurtc_base = of_iomap(node, 0);
	if (!hisi_pmurtc_base) {
		pr_err("sim_pmu_hpd_init: of_iomap failed\n");
		goto err;
	}
    ret = 0;

err:
	pr_info("%s: finished!\n", __func__);
	return ret;
}

static ssize_t sim0_hotplug_state_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE - 1, "%d\n", sim0_state);
}

static ssize_t sim0_hotplug_state_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	int val;

	if (kstrtoint(buf, 10, &val))
	{
		pr_info("%s: EINVAL!\n", __func__);
		return -EINVAL;
	}

	pr_info("%s: sim0_state=%d, val=%d \n", __func__, sim0_state, val);

	sim0_state = val;

	return (ssize_t)count;
}

static ssize_t sim1_hotplug_state_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE - 1, "%d\n", sim1_state);
}

static ssize_t sim1_hotplug_state_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	int val;

	if (kstrtoint(buf, 10, &val))
	{
		pr_info("%s: EINVAL!\n", __func__);
		return -EINVAL;
	}

	pr_info("%s: sim1_state=%d, val=%d \n", __func__, sim1_state, val);

	sim1_state = val;

	return (ssize_t)count;
}

static struct kobj_attribute sim0_attribute =
	__ATTR(sim0_hotplug_state, 0664, sim0_hotplug_state_show, sim0_hotplug_state_store);

static struct kobj_attribute sim1_attribute =
	__ATTR(sim1_hotplug_state, 0664, sim1_hotplug_state_show, sim1_hotplug_state_store);

static struct attribute *attrs[] = {
	&sim0_attribute.attr,
	&sim1_attribute.attr,
	NULL
};

static struct attribute_group sim_attr_group = {
	.attrs = attrs
};

static int sys_add_sim_node(void)
{
	int retval;
	pr_info("%s, begin\n", __func__);

	/* wait for kernel_kobj node ready: */
	while (kernel_kobj == NULL)
	{
		msleep(1000);
	}

	/* Create kobject named "sim",located under /sys/kernel/ */
	sim_kobj = kobject_create_and_add("sim", kernel_kobj);
	if (!sim_kobj)
	{
		return -ENOMEM;
	}

	/* Create the files associated with this kobject */
	retval = sysfs_create_group(sim_kobj, &sim_attr_group);
	if (retval)
	{
		kobject_put(sim_kobj);
	}

	pr_info("%s, end, retval=%d\n", __func__, retval);
	return retval;
}

static void update_sim_state_info(int sim_id, int sim_state)
{
    if (SIM0_ID == sim_id)
    {
        if (sim_state == SIM_CARD_IN)
        {
            sim0_hotplug_state_store(sim_kobj, attrs[SIM0_ID], STR_SIM_CARD_IN, strlen(STR_SIM_CARD_IN));
        }
        else
        {
            sim0_hotplug_state_store(sim_kobj, attrs[SIM0_ID], STR_CLEAR_SIM_CARD_STATE, strlen(STR_CLEAR_SIM_CARD_STATE));
        }
    }
    else if (SIM1_ID == sim_id)
    {
        if (sim_state == SIM_CARD_IN)
        {
            sim1_hotplug_state_store(sim_kobj, attrs[SIM1_ID], STR_SIM_CARD_IN, strlen(STR_SIM_CARD_IN));
        }
        else
        {
            sim1_hotplug_state_store(sim_kobj, attrs[SIM1_ID], STR_CLEAR_SIM_CARD_STATE, strlen(STR_CLEAR_SIM_CARD_STATE));
        }
    }
    else
    {
        pr_err("%s, invalid sim_id:%d\n", __func__, sim_id);
    }
}

static void set_sim_status(struct hisi_sim_hotplug_info *info, u32 sim_no, u32 sim_status, u32 trace, u32 notify)
{
	struct timeval time;
	sim_log log_temp;
	memset((void *)&log_temp, 0, sizeof(log_temp));

	do_gettimeofday(&time);
	log_temp.time = time.tv_sec;
	log_temp.sim_no = sim_no;
	log_temp.sim_status = sim_status;
	log_temp.hpd_level = -1;
	log_temp.det_level = gpio_get_value(info->det_gpio);
	log_temp.trace = trace;

	if (SIM0_ID == info->sim_id) {
		log_temp.sim_mux[0] = readl(info->iomg_base + GPIO103_IOMG_ADDR);
		log_temp.sim_mux[1] = readl(info->iomg_base + GPIO104_IOMG_ADDR);
		log_temp.sim_mux[2] = readl(info->iomg_base + GPIO105_IOMG_ADDR);
	} else if (SIM1_ID == info->sim_id) {
		log_temp.sim_mux[0] = readl(info->iomg_base + GPIO106_IOMG_ADDR);
		log_temp.sim_mux[1] = readl(info->iomg_base + GPIO107_IOMG_ADDR);
		log_temp.sim_mux[2] = readl(info->iomg_base + GPIO108_IOMG_ADDR);
	} else {
		pr_err("%s: sim id [%d] is err!\n", __func__, info->sim_id);
	}

	if (SIM1_ID == info->sim_id && info->card_tray == TWO_CARDS_IN_ONE_TRAY) {
		pr_info("%s: sim1, TWO_CARDS_IN_ONE_TRAY, do not add to log history\n", __func__);
	} else {
		if (g_sim_log_idx > MAX_SIM_HOTPLUG_LOG - 1) {
			g_sim_log_idx = 0;
		}

		memcpy(&g_log_org[g_sim_log_idx], &log_temp, sizeof(sim_log));

		g_sim_log_idx++;
	}

	pr_info("sim%d set:%d det_level:%d\n", sim_no, sim_status, log_temp.det_level);
}

static int sim_set_active(struct hisi_sim_hotplug_info *info)
{
	int err = 0;
	if (1 == info->set_gpio) {
		err = pinctrl_select_state(info->pctrl, info->sim_default);
		if (err < 0) {
			pr_err("set iomux normal error, %d\n", err);
			return err;
		}
	}
	pr_info("[%s]:set iomux normal state ok!\n", __func__);
	return 0;
}

static int sim_set_inactive(struct hisi_sim_hotplug_info *info)
{
	int err = 0;
	if (1 == info->set_gpio) {
		err = pinctrl_select_state(info->pctrl, info->sim_idle);
		if (err < 0) {
			pr_err("set iomux idle error, %d\n", err);
			return err;
		}
	}
	pr_info("[%s]:set iomux idle state ok!\n", __func__);

	return 0;
}

static void hisi_sim_det_msg_to_ccore(struct hisi_sim_hotplug_info *info, u8 sim_state)
{
#ifdef CONFIG_HISI_BALONG_MODEM
	u32 channel_id = 0;
	int det_gpio_state = 0;

	if (NULL == info)
	{
		pr_err("%s: input param is null, just return.\n", __func__);
		return;
	}

	if (0 == info->send_msg_to_lpm3)
	{
		pr_err("%s: send_msg_to_lpm3 is 0.\n", __func__);
		return;
	}

	det_gpio_state = gpio_get_value(info->det_gpio);
	pr_info("%s, sim_id=%d, old_det_gpio=%d, det_gpio=%d \n", __func__, info->sim_id, info->old_det_gpio_state, det_gpio_state);
	if (det_gpio_state == info->old_det_gpio_state) {
		pr_info("%s, det_gpio_state not changed, just return \n", __func__);
		return;
	}
	info->old_det_gpio_state = det_gpio_state;

	if (SIM0_ID == info->sim_id)
	{
		channel_id = SIM0_CHANNEL_ID;
	}
	else if (SIM1_ID == info->sim_id)
	{
		channel_id = SIM1_CHANNEL_ID;
	}
	else
	{
		pr_err("%s: sim_id invalid:%d.\n", __func__, info->sim_id);
		return;
	}

	if (SIM_CARD_OUT != sim_state && SIM_CARD_IN != sim_state)
	{
		pr_err("%s: sim_state invalid and it is %d.\n", __func__, sim_state);
		return;
	}

	pr_info("%s: sim_id=%d, sim_state=%s \n", __func__, info->sim_id, s_sim_state_tbl[sim_state].sim_state_str);
	bsp_icc_send(ICC_CPU_MODEM, channel_id, &sim_state, sizeof(sim_state));
	update_sim_state_info(info->sim_id, sim_state);
	pr_info("%s: end \n", __func__);
#else
	pr_info("%s: CONFIG_HISI_BALONG_MODEM not defined \n", __func__);
#endif
}

static void sim_det_irq_set_active(struct hisi_sim_hotplug_info *sim_hotplug_info)
{
	sim_hotplug_info->sim_pluged = SIM_CARD_IN;
	sim_set_active(sim_hotplug_info);
	pr_info("%s, %d, sim_hotplug_info->sim_id=%d, SIM_CARD_IN \n",
			__func__, __LINE__, sim_hotplug_info->sim_id);

	hisi_sim_det_msg_to_ccore(sim_hotplug_info, SIM_CARD_IN);

	set_sim_status(sim_hotplug_info, sim_hotplug_info->sim_id, sim_hotplug_info->sim_pluged, 0, NO_NOTIFY);
}

static void sim_det_irq_set_deactive(struct hisi_sim_hotplug_info *sim_hotplug_info)
{
	sim_hotplug_info->sim_pluged = SIM_CARD_OUT;
	sim_set_inactive(sim_hotplug_info);
	pr_info("%s, %d, sim_hotplug_info->sim_id=%d, SIM_CARD_OUT \n",
			__func__, __LINE__, sim_hotplug_info->sim_id);

	hisi_sim_det_msg_to_ccore(sim_hotplug_info, SIM_CARD_OUT);

	set_sim_status(sim_hotplug_info, sim_hotplug_info->sim_id, sim_hotplug_info->sim_pluged, 1, NO_NOTIFY);
}

static void sim_manage_sim_state(int det_gpio_state, struct hisi_sim_hotplug_info *sim_hotplug_info)
{
    pr_info("[%s], sim_id=%d, det_gpio_state:%d, old_det_gpio_state:%d\n", __func__, sim_hotplug_info->sim_id, det_gpio_state, sim_hotplug_info->old_det_gpio_state);

	if (!det_gpio_state) {
		if (NORMAL_OPEN == sim_hotplug_info->det_direction && sim_hotplug_info->sim_pluged == SIM_CARD_OUT) {
            sim_det_irq_set_active(sim_hotplug_info);
        } else if (NORMAL_CLOSE == sim_hotplug_info->det_direction &&  sim_hotplug_info->sim_pluged == SIM_CARD_IN) {
    		sim_det_irq_set_deactive(sim_hotplug_info);
		}
	} else {
		if (NORMAL_OPEN == sim_hotplug_info->det_direction && sim_hotplug_info->sim_pluged == SIM_CARD_IN) {
    		sim_det_irq_set_deactive(sim_hotplug_info);
        } else if (NORMAL_CLOSE == sim_hotplug_info->det_direction && sim_hotplug_info->sim_pluged == SIM_CARD_OUT) {
    		sim_det_irq_set_active(sim_hotplug_info);
		}
	}
}

static void inquiry_sim_det_irq_reg(struct work_struct *work)
{
	struct hisi_sim_hotplug_info *sim_hotplug_info = container_of(work, struct hisi_sim_hotplug_info, sim_hotplug_det_wk1);
	int det_gpio_state = 0;

	det_gpio_state = gpio_get_value(sim_hotplug_info->det_gpio);
	pr_info("%s, %d, sim_id:%d, det_gpio_state:%d, sim_pluged:%d, send_msg_to_lpm3:%d\n",
		__func__, __LINE__, sim_hotplug_info->sim_id, det_gpio_state, sim_hotplug_info->sim_pluged, sim_hotplug_info->send_msg_to_lpm3);

    if (1 == sim_hotplug_info->send_msg_to_lpm3) {
        if (0 == sim_hotplug_info->debounce_checking && NULL != sim_hotplug_info->sim_debounce_delay_wq) {
            sim_hotplug_info->debounce_checking = 1;
            queue_delayed_work(sim_hotplug_info->sim_debounce_delay_wq,
                               &sim_hotplug_info->sim_debounce_delay_work,
				               msecs_to_jiffies(sim_hotplug_info->debounce_wait_time));
            pr_info("%s, sim_id:%d, debounce check begin \n", __func__, sim_hotplug_info->sim_id);
        }
        else
        {
            pr_info("%s, sim_id:%d, debounce check ongoing, just return \n", __func__, sim_hotplug_info->sim_id);
        }

        return;
    }

    sim_manage_sim_state(det_gpio_state, sim_hotplug_info);
}

static irqreturn_t sim_det_irq_handler(int irq, void *data)
{
	struct hisi_sim_hotplug_info *sim_hotplug_info = data;
	if (sim_hotplug_info == NULL) {
		pr_info("[%s]:%d get irq data is error\n", __func__, __LINE__);
		return IRQ_HANDLED;
	}

	queue_work(sim_hotplug_info->sim_hotplug_det_wq, &sim_hotplug_info->sim_hotplug_det_wk1);

	return IRQ_HANDLED;
}

static int sim_read_pmu_settings(struct hisi_sim_hotplug_info *info, struct device_node *np)
{
    int ret                 = 0;
    u32 card_tray           = 0;
    u32 hpd_trigger         = 0;
    u32 debounce_settings   = 0;

    pr_info("[%s] begin \n",__func__);

    ret = of_property_read_u32(np, "card_tray", &card_tray);
    if (ret < 0) {
        pr_err("failed to read card_tray\n");
        return ret;
    }
    info->card_tray = card_tray;

    ret = of_property_read_u32(np, "hpd_trigger", &hpd_trigger);
    if (ret < 0) {
        pr_err("failed to read hpd_trigger\n");
        return ret;
    }
    info->hpd_trigger = hpd_trigger;

    ret = of_property_read_u32(np, "debounce_settings", &debounce_settings);
    if (ret < 0) {
        pr_err("failed to read debounce_settings\n");
        return ret;
    }
    info->debounce_settings = debounce_settings;

    pr_info("[%s] end, ret=%d, card_tray=%d, hpd_trigger=0x%x, debounce_settings=0x%x \n",
            __func__, ret, card_tray, hpd_trigger, debounce_settings);
    return ret;
}

static void sim_debounce_check_fun(struct work_struct *work)
{
	struct hisi_sim_hotplug_info *sim_hotplug_info = container_of(work, struct hisi_sim_hotplug_info, sim_debounce_delay_work.work);
	int det_gpio_state = 0;

	det_gpio_state = gpio_get_value(sim_hotplug_info->det_gpio);
	pr_info("%s, %d, sim_id,%d, det_gpio_state:%d, sim_pluged:%d\n",
		__func__, __LINE__, sim_hotplug_info->sim_id, det_gpio_state, sim_hotplug_info->sim_pluged);

    sim_manage_sim_state(det_gpio_state, sim_hotplug_info);

    sim_hotplug_info->debounce_checking = 0;
    pr_info("%s, sim_id:%d, debounce check end \n", __func__, sim_hotplug_info->sim_id);
}

static int sim_init_debounce_check_wq(struct hisi_sim_hotplug_info *info)
{
    int ret = 0;

    if (NULL == info) {
        pr_err("[%s] info is NULL, just return \n", __func__);
        ret = -EINVAL;
    } else {
        pr_info("[%s], init delay work for sim %d debounce check \n", __func__, info->sim_id);
        if (1 == info->send_msg_to_lpm3) {
            info->debounce_checking = 0;
            info->sim_debounce_delay_wq = create_singlethread_workqueue(work_name[info->sim_id]);
            if (NULL == info->sim_debounce_delay_wq) {
                pr_err("[%s]: sim_debounce_delay_wq create failed. \n", __func__);
                ret = -ENOMEM;
                goto error;
            }

            INIT_DELAYED_WORK(&(info->sim_debounce_delay_work), sim_debounce_check_fun);
        }
    }

error:
    pr_info("[%s], ret=%d, end \n", __func__, ret);
    return ret;
}

static int sim_hotplug_dt_init(struct hisi_sim_hotplug_info *info, struct device_node *np, struct device *dev)
{
	int ret = 0;
	enum of_gpio_flags flags;
	unsigned int sim_info[2] = {0};
	u32 sim_id = 0;
    u32 send_msg_to_lpm3 = 0;
    const char *det_pin_direction = NULL;
	int  shared_irq = 0;
    u32 debounce_wait_time = 0;

	const char *sim_status = NULL;
	ret = of_property_read_string(np, "set_gpio", &sim_status);
	if (ret < 0) {
		pr_err("[%s] failed to read set_gpio\n",__func__);
		return ret;
	}
	if (0 == strcmp(sim_status, "ok")) {
		info->set_gpio = 1;
	} else if (0 == strcmp(sim_status, "disabled")) {
		info->set_gpio = 0;
	} else {
		info->set_gpio = 2;
	}
	pr_info("[%s] set_gpio=%d\n",__func__, info->set_gpio);
	if (2 == info->set_gpio) {
		pr_err("[%s] set_gpio=2 and return\n",__func__);
		return -EINVAL;
	}

	ret = of_property_read_string(np, "detect_pin_direction", &det_pin_direction);
	if (ret < 0) {
		pr_err("[%s] failed to read detect_pin_direction\n",__func__);
		return ret;
	}
	if (0 == strcmp(det_pin_direction, "open")) {
		info->det_direction = NORMAL_OPEN;
	} else if (0 == strcmp(det_pin_direction, "closed")) {
		info->det_direction = NORMAL_CLOSE;
	} else {
		info->det_direction = NORMAL_BUTT;
	}
	pr_info("[%s] detect_pin_direction=%d\n",__func__, info->det_direction);
	if (NORMAL_BUTT == info->det_direction) {
		pr_info("[%s] detect_pin_direction=2 and set to 1\n",__func__);
		info->det_direction = NORMAL_OPEN;
	}

	ret = of_property_read_u32_array(np, "ap_to_lpm3_sim_input",
						sim_info, 2);
	if (ret) {
		dev_err(dev, "no ap_to_lpm3_sim_input state\n");
		return ret;
	}
	info->sim_info.sim_msg_input_value = sim_info[0];
	info->sim_info.sim_msg_input_flag = sim_info[1];

	ret = of_property_read_u32_array(np, "ap_to_lpm3_sim_output",
						sim_info, 2);
	if (ret) {
		dev_err(dev, "no ap_to_lpm3_sim_output state\n");
		return ret;
	}
	info->sim_info.sim_msg_output_value = sim_info[0];
	info->sim_info.sim_msg_output_flag = sim_info[1];

	ret = of_property_read_u32_array(np, "sim_id",
						&sim_id, 1);
	if (ret) {
		dev_err(dev, "no sim hotplug sim_id\n");
		return ret;
	}
	info->sim_id = sim_id;

	/*set det gpio to irq*/
	info->det_gpio = of_get_gpio_flags(np, 0, &flags);
    pr_info("[%s] info->det_gpio=%d\n",__func__, info->det_gpio);
	if (info->det_gpio < 0)
		return info->det_gpio;

	if (!gpio_is_valid(info->det_gpio))
		return -EINVAL;

	if (of_property_read_u32(np, "shared-irq", &shared_irq))
	{
		dev_info(dev, "shared-irq property not found, using "
		"value of 0 as default\n");
		shared_irq = 0;
	}

	if(0 == shared_irq)
	{
		ret = gpio_request_one(info->det_gpio, GPIOF_IN, "sim_det");
		if (ret < 0) {
			pr_err("failed to request gpio%d\n", info->det_gpio);
			return ret;
		}
	}

	info->det_irq = gpio_to_irq(info->det_gpio);

	ret = of_property_read_u32(np, "send_msg_to_lpm3", &send_msg_to_lpm3);
	if (ret < 0) {
		pr_err("failed to read send_msg_to_lpm3\n");
		return ret;
	}
    info->send_msg_to_lpm3 = send_msg_to_lpm3;

	/*create det workqueue*/
	info->sim_hotplug_det_wq = create_singlethread_workqueue("sim_hotplug_det");
	if (info->sim_hotplug_det_wq == NULL) {
		pr_err("failed to create work queue\n");
		return -ENOMEM;
	}

	INIT_WORK(&info->sim_hotplug_det_wk1, (void *)inquiry_sim_det_irq_reg);

    if (1 == send_msg_to_lpm3)
    {
        ret = of_property_read_u32(np, "debounce_wait_time", &debounce_wait_time);
        if (ret < 0) {
            debounce_wait_time = SIM_DEBOUNCE_CHECK_WAIT_TIME;
        }

        info->debounce_wait_time = debounce_wait_time;
        pr_info("%s, debounce_wait_time=%d \n", __func__, debounce_wait_time);
    }

    ret = sim_init_debounce_check_wq(info);

	return ret;
}

static int sim_state_init(struct hisi_sim_hotplug_info *info, struct device *dev)
{
	int err = 0;

	info->pctrl = devm_pinctrl_get(dev);
	if (IS_ERR(info->pctrl)) {
		dev_err(dev, "failed to devm pinctrl get\n");
		err = -EINVAL;
		return err;
	}

	info->sim_default = pinctrl_lookup_state(info->pctrl, PINCTRL_STATE_DEFAULT);
	if (IS_ERR(info->sim_default)) {
		dev_err(dev, "failed to pinctrl lookup state default\n");
		err = -EINVAL;
		return err;
	}

	info->sim_idle = pinctrl_lookup_state(info->pctrl, PINCTRL_STATE_IDLE);
	if (IS_ERR(info->sim_idle)) {
		dev_err(dev, "failed to pinctrl lookup state idle\n");
		err = -EINVAL;
		return err;
	}

	if (!gpio_get_value(info->det_gpio)) {
		info->old_det_gpio_state = 0;
		if (NORMAL_CLOSE == info->det_direction) {
			info->sim_pluged = SIM_CARD_OUT;
			sim_set_inactive(info);
			set_sim_status(info, info->sim_id, info->sim_pluged, 3, NO_NOTIFY);
		} else {
			info->sim_pluged = SIM_CARD_IN;
			sim_set_active(info);
			set_sim_status(info, info->sim_id, info->sim_pluged, 2, NO_NOTIFY);
		}
	} else {
		info->old_det_gpio_state = 1;
		if (NORMAL_CLOSE == info->det_direction) {
			info->sim_pluged = SIM_CARD_IN;
			sim_set_active(info);
			set_sim_status(info, info->sim_id, info->sim_pluged, 2, NO_NOTIFY);
		} else {
			info->sim_pluged = SIM_CARD_OUT;
			sim_set_inactive(info);
			set_sim_status(info, info->sim_id, info->sim_pluged, 3, NO_NOTIFY);
		}
	}

	mutex_init(&(info->sim_hotplug_lock));
	wake_lock_init(&info->sim_hotplug_wklock, WAKE_LOCK_SUSPEND, "android-simhotplug");

	return 0;
}

static int get_iomux_base_addr(struct hisi_sim_hotplug_info *info)
{
	struct device_node *np = NULL;

	np = of_find_compatible_node(NULL, NULL, "pinctrl-single0");
	if (!np) {
		pr_err("failed to find pinctrl-single0 node!\n");
		return -1;
	}
	info->iomg_base = of_iomap(np, 0);
	if (!info->iomg_base) {
		pr_err("unable to map pinctrl-single0 dist registers!\n");
		return -1;
	}

	return 0;
}

static int hisi_sim_hotplug_probe(struct platform_device *pdev)
{
	struct hisi_sim_hotplug_info *info;
	struct device_node *np = NULL;
	struct device *dev = NULL;
	int ret = 0;
#ifdef CONFIG_HISI_RDR
	static int g_rdr_flag = 0;
	u32 rdr_int = 0;
#endif
	int shared_irq = 0;

	if (pdev == NULL) {
		pr_err("[%s]sim_hotplug get  platform device para is err!\n", __func__);
		return -EINVAL;
	}

	dev = &pdev->dev;
	np = dev->of_node;

	info = devm_kzalloc(&pdev->dev, sizeof(*info), GFP_KERNEL);
	if (!info) {
		dev_err(&pdev->dev, "failed to allocate memory\n");
		return -ENOMEM;
	}

	ret = get_iomux_base_addr(info);
	if (ret < 0) {
		dev_err(&pdev->dev, "failed to get iomux base addr\n");
		return ret;
	}

#ifdef CONFIG_HISI_RDR
	if (g_rdr_flag == 0) {
		RDR_ASSERT(rdr_afreg(rdr_int, RDR_LOG_SIMHOTPLUG_BUF, RDR_U32, LOG_SIMHOTPLUG_BUF_LEN));
		g_log_org = (sim_log *)field_addr(u32, rdr_int);
		memset(g_log_org, 0xFF, MAX_SIM_HOTPLUG_LOG * sizeof(sim_log));
		g_rdr_flag = 1;

		pr_info("%s, g_log_org=0x%.8x, rdr_int=0x%x\n", __func__, g_log_org, rdr_int);
	}
#endif

	ret = sim_pmu_hpd_init();
	if (ret < 0) {
		dev_err(&pdev->dev, "failed to sim_pmu_hpd_init\n");
		return ret;
	}

    sim_read_pmu_settings(info, np);
    sim_pmu_hpd_write(info);
    sim_pmu_hpd_read();

	ret = sim_hotplug_dt_init(info, np, dev);
	if (ret < 0) {
		dev_err(&pdev->dev, "failed to set sim hotplug gpio hw init\n");
		return ret;
	}

	ret = sim_state_init(info, dev);
	if (ret < 0) {
		dev_err(&pdev->dev, "failed to set sim state hw init\n");
		goto free_sim_det_wq;
	}

	if (of_property_read_u32(np, "shared-irq", &shared_irq))
	{
		dev_info(dev, "shared-irq property not found, using "
		"value of 0 as default\n");
		shared_irq = 0;
	}

	if (shared_irq)
	{
		ret = request_threaded_irq(info->det_irq, sim_det_irq_handler, NULL,
				IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING | IRQF_NO_SUSPEND | IRQF_SHARED,
				"sim_det", info);
	}
	else
	{
		ret = request_threaded_irq(info->det_irq, sim_det_irq_handler, NULL,
				IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING | IRQF_NO_SUSPEND,
				   "sim_det", info);
	}

	if (ret < 0) {
		dev_err(&pdev->dev, "failed to requset sim_det irq!\n");
		goto free_sim_lock;
	}

	if (0 == info->sim_id) {
		sys_add_sim_node();
	}

	platform_set_drvdata(pdev, info);

	return ret;

free_sim_lock:
	wake_lock_destroy(&info->sim_hotplug_wklock);
	mutex_destroy(&info->sim_hotplug_lock);
free_sim_det_wq:
	if (info->sim_hotplug_det_wq)
		destroy_workqueue(info->sim_hotplug_det_wq);

	return ret;
}

static int hisi_sim_hotplug_remove(struct platform_device *pdev)
{
	struct hisi_sim_hotplug_info *info;

	/*get hisi_sim_hotplug_info*/
	info = platform_get_drvdata(pdev);
	if (NULL == info) {
		pr_err("%s %d platform_get_drvdata NULL\n", __func__, __LINE__);
		return -1;
	}

	mutex_destroy(&info->sim_hotplug_lock);
	wake_lock_destroy(&info->sim_hotplug_wklock);

	if (info->sim_hotplug_det_wq)
		destroy_workqueue(info->sim_hotplug_det_wq);

    if (1 == info->send_msg_to_lpm3 && info->sim_debounce_delay_wq) {
        pr_info("[%s], remove delay work for sim %d \n", __func__, info->sim_id);
		cancel_delayed_work(&info->sim_debounce_delay_work);
		flush_workqueue(info->sim_debounce_delay_wq);
		destroy_workqueue(info->sim_debounce_delay_wq);
    }

	return 0;
}

static struct of_device_id hisi_sim_hotplug_of_match[] = {
	{ .compatible = "hisilicon,sim-hotplug0", },
	{ .compatible = "hisilicon,sim-hotplug1", },
	{ },
};
MODULE_DEVICE_TABLE(of, hisi_sim_hotplug_of_match);

#ifdef CONFIG_PM
static int hisi_sim_hotplug_suspend(struct platform_device *pdev, pm_message_t state)
{
	struct hisi_sim_hotplug_info *info;

	pr_info("[%s]:suspend+\n", __func__);
	/*get hisi_sim_hotplug_info*/
	info = platform_get_drvdata(pdev);
	if (NULL == info) {
		pr_err("%s %d platform_get_drvdata NULL\n", __func__, __LINE__);
		return -1;
	}

	if (!mutex_trylock(&info->sim_hotplug_lock)) {
		dev_err(&pdev->dev, "%s: mutex_trylock.\n", __func__);
		return -EAGAIN;
	}
	pr_info("[%s]:suspend-\n", __func__);
	return 0;
}

static int hisi_sim_hotplug_resume(struct platform_device *pdev)
{
	struct hisi_sim_hotplug_info *info;

	pr_info("[%s]:resume+\n", __func__);
	/*get hisi_sim_hotplug_info*/
	info = platform_get_drvdata(pdev);
	if (NULL == info) {
		pr_err("%s %d platform_get_drvdata NULL\n", __func__, __LINE__);
		return -1;
	}

	mutex_unlock(&(info->sim_hotplug_lock));
	pr_info("[%s]:resume-\n", __func__);
	return 0;
}
#endif

static struct platform_driver hisi_sim_hotplug_driver = {
	.probe		= hisi_sim_hotplug_probe,
	.remove		= hisi_sim_hotplug_remove,
	.driver		= {
		.owner		= THIS_MODULE,
		.name		= "hisi-sim_hotplug",
		.of_match_table	= hisi_sim_hotplug_of_match,
	},
#ifdef CONFIG_PM
	.suspend        = hisi_sim_hotplug_suspend,
	.resume         = hisi_sim_hotplug_resume,
#endif
};

int __init hisi_sim_hotplug_init(void)
{
	return platform_driver_register(&hisi_sim_hotplug_driver);
}

static void __exit hisi_sim_hotplug_exit(void)
{
	platform_driver_unregister(&hisi_sim_hotplug_driver);
}

late_initcall(hisi_sim_hotplug_init);
module_exit(hisi_sim_hotplug_exit);

MODULE_DESCRIPTION("Sim hotplug driver");
MODULE_LICENSE("GPL v2");
