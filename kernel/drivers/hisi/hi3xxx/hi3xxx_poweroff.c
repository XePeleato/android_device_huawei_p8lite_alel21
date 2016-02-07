/*
 * Power off or Reboot the hi3xxx device.
 *
 * Copyright (C) 2014 Hisilicon, Young Choi <cuiyong1@hisilicon.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <asm/cacheflush.h>
#include <asm/psci.h>


static void __iomem *sysctrl_base;
extern void (*pm_power_off)(void);
extern void (*arm_pm_restart)(char str, const char *cmd);
extern struct psci_operations psci_ops;

#ifdef CONFIG_HISI_REBOOT_TYPE
static void __iomem *pmu_base;
/* be careful:: shoule be the same as defined in fastboot*/
/* there are only 4 bit for reboot_type;
 * U can use the reserved number to add the new command*/
enum {
	REBOOT_BOOTLOADER_CMDNUM = 1,   /*1*/
	REBOOT_RECOVERY_CMDNUM,         /*2*/
	REBOOT_RESETFACTORY_CMDNUM,     /*3*/
	REBOOT_RESETUSER_CMDNUM,        /*4*/
	REBOOT_SDUPDATE_CMDNUM,         /*5*/
	REBOOT_PANIC_CMDNUM,            /*6*/
	REBOOT_RESIZE_CMDNUM,           /*7*/
	REBOOT_RES_1,                   /*8*/
	REBOOT_USBUPDATE_CMDNUM,        /*9*/
	REBOOT_CUST_CMDNUM,                     /*10*/
	REBOOT_USERSDUPDATE_CMDNUM,                     /*11*/
	REBOOT_OEM_RTC_CMDNUM,          /*12*/
	/*be careful: the define of cold reboot is different from k3v2*/
	REBOOT_COLDBOOT_CMDNUM,         /*13*/
	REBOOT_MOUNTFAIL_CMDNUM,                /*14*/
	/*be careful: this is the new one in k3v3*/
	REBOOT_WATCHDOG_CMDNUM,         /*15*/
};

#define REBOOT_COLDBOOT_STR            "coldboot"
#define REBOOT_BOOTLOADER_STR          "bootloader"
#define REBOOT_RECOVERY_STR            "recovery"
#define REBOOT_RESETUSER_STR           "resetuser"
#define REBOOT_RESETFACTORY_STR                "resetfactory"
#define REBOOT_SDUPDATE_STR            "sdupdate"
#define REBOOT_USBUPDATE_STR           "usbupdate"
#define REBOOT_PANIC_STR               "panic"
#define REBOOT_RESIZE_STR              "resize"
#define REBOOT_WATCHDOG_STR            "watchdog"
#define REBOOT_CUST_STR                "cust"
#define REBOOT_USERSDUPDATE_STR                "usersdupdate"
#define REBOOT_OEM_RTC_STR             "oem_rtc"
#define REBOOT_MOUNTFAIL_STR   "mountfail"

struct reboot_cmdword {
	char *name;
	unsigned int num;
};

static struct reboot_cmdword reboot_cmd_map[] = {
	{ REBOOT_COLDBOOT_STR, REBOOT_COLDBOOT_CMDNUM },
	{ REBOOT_BOOTLOADER_STR, REBOOT_BOOTLOADER_CMDNUM },
	{ REBOOT_RECOVERY_STR, REBOOT_RECOVERY_CMDNUM },
	{ REBOOT_RESETFACTORY_STR, REBOOT_RESETFACTORY_CMDNUM },
	{ REBOOT_RESETUSER_STR, REBOOT_RESETUSER_CMDNUM },
	{ REBOOT_SDUPDATE_STR, REBOOT_SDUPDATE_CMDNUM },
	{ REBOOT_USBUPDATE_STR, REBOOT_USBUPDATE_CMDNUM },
	{ REBOOT_PANIC_STR, REBOOT_PANIC_CMDNUM },
	{ REBOOT_RESIZE_STR, REBOOT_RESIZE_CMDNUM },
	{ REBOOT_WATCHDOG_STR, REBOOT_WATCHDOG_CMDNUM },
	{ REBOOT_CUST_STR, REBOOT_CUST_CMDNUM },
	{ REBOOT_USERSDUPDATE_STR, REBOOT_USERSDUPDATE_CMDNUM },
	{ REBOOT_OEM_RTC_STR, REBOOT_OEM_RTC_CMDNUM },
	{ REBOOT_MOUNTFAIL_STR, REBOOT_MOUNTFAIL_CMDNUM },
};

static unsigned int find_cmd_num(const char *cmd)
{
	unsigned int i;
	unsigned int cmd_id = REBOOT_COLDBOOT_CMDNUM;

	for (i=0; i<sizeof(reboot_cmd_map)/sizeof(reboot_cmd_map[0]); i++) {
		if (!strcmp(reboot_cmd_map[i].name, cmd)) {
			cmd_id = reboot_cmd_map[i].num;
			break;
		}
	}
	return cmd_id;
}
/*lint -e750*/
#define REG_SYSCTRL_SCLPM3CTRL         0x510
#define PMU_RESET_REG                          (0x115 << 2)
/*lint +e750*/

static unsigned int panic_flag = 0;
static unsigned int watchdog_flag = 0;
/*lint -e750*/
#define PMU_REG_IRQ2           (0x121 << 2)
#define PMU_IRQ_BOI_R          0x04
#define PMU_REBOOT_STATE_ADDR  (0x1c7 << 2)
#define PMU_REBOOT_STATE_MASK  0x20
/*lint +e750*/


static int get_pmu_addr(void)
{
	struct device_node *np;

	if (pmu_base == NULL) {
		np = of_find_compatible_node(NULL, NULL, "hisilicon,hi6421-pmic");
		if (!np) {
			printk(KERN_ERR "get pmu addr error !\n");
			return -1;
		}

		pmu_base = of_iomap(np, 0);
		if (pmu_base == NULL) {
			printk(KERN_ERR "get pmu addr error !\n");
			return -1;
		}
	}

	printk("get_pmu_addr ok\n");

	return 0;
}

int get_str_len(char *str)
{
	int count = 0;
	char *p = str;

	while ((*p != ',') && (*p != '\0'))     {
		count++;
		p++;
	}

	return count;
}

void set_panic_resetflag(void)
{
	panic_flag = 1;
}
EXPORT_SYMBOL(set_panic_resetflag);

void set_watchdog_resetflag(void)
{
	watchdog_flag = 1;
}
EXPORT_SYMBOL(set_watchdog_resetflag);

static int is_panic_reboot(void)
{
	return panic_flag;
}

static int is_watchdog_reboot(void)
{
	return watchdog_flag;
}

static unsigned int get_reboot_id(const char *cmd)
{
	unsigned int cmd_id;

	/* case: warm boot */
	if (cmd) {
		cmd_id = (find_cmd_num(cmd));
	} else {
		/* otherwise cold boot */
		if (is_panic_reboot() != 0) {
			cmd_id = find_cmd_num(REBOOT_PANIC_STR);
		} else if (is_watchdog_reboot() != 0) {
			cmd_id = find_cmd_num(REBOOT_WATCHDOG_STR);
		}else {
			cmd_id = find_cmd_num(REBOOT_COLDBOOT_STR);
		}
	}

	return cmd_id;
}
#endif

void set_pmuon_mode(void)
{
	if (sysctrl_base)
		writel(0x80000000, sysctrl_base + 0x84);
}

void hisi_pm_system_off(void)
{
	BUG_ON(!psci_ops.system_off);

	psci_ops.system_off();

	while(1)
		asm volatile ("wfi");

	return ;
}

void hisi_pm_system_reset(char mode, const char *cmd)
{
	unsigned int cmd_id = 13;

#ifdef CONFIG_HISI_REBOOT_TYPE
	unsigned int value;
	/* get reboot flag */
	cmd_id = get_reboot_id(cmd);

	/* write reboot flag */
	if (pmu_base) {
		value = readl(pmu_base + PMU_RESET_REG);
		value &= (0x1<<7);
		value |= cmd_id;
		writel(value, (pmu_base + PMU_RESET_REG));
	}
	if (cmd_id == REBOOT_PANIC_CMDNUM || cmd_id == REBOOT_WATCHDOG_CMDNUM)
		flush_cache_all();
#endif

	set_pmuon_mode();

	BUG_ON(!psci_ops.system_reset);
	psci_ops.system_reset(cmd_id);

	/*wait for restart*/
	while(1);
}
static int hi3xxx_reset_probe(struct platform_device *pdev)
{
	struct device_node *np = NULL;

	np = of_find_compatible_node(NULL, NULL, "hisilicon,sysctrl");
	if (!np) {
		pr_err("%s: hisilicon,sysctrl No compatible node found\n", __func__);
		return -ENODEV;
	}

	sysctrl_base = of_iomap(np, 0);
	if (!sysctrl_base) {
		pr_err("%s: hisilicon,sysctrl_base is NULL\n", __func__);
		return -ENODEV;
	}

#ifdef CONFIG_HISI_REBOOT_TYPE
	get_pmu_addr();
#endif

	pm_power_off = hisi_pm_system_off;
	arm_pm_restart = hisi_pm_system_reset;

	return 0;
}

static int hi3xxx_reset_remove(struct platform_device *pdev)
{
	if (pm_power_off == hisi_pm_system_off)
		pm_power_off = NULL;

	if (arm_pm_restart == hisi_pm_system_reset)
		arm_pm_restart = NULL;

	return 0;
}

static const struct of_device_id of_hi3xxx_reset_match[] = {
	{ .compatible = "hisilicon,hi3xxx-reset", },
	{},
};

static struct platform_driver hi3xxx_reset_driver = {
	.probe = hi3xxx_reset_probe,
	.remove = hi3xxx_reset_remove,
	.driver = {
		.name = "hi3xxx-reset",
		.owner = THIS_MODULE,
		.of_match_table = of_hi3xxx_reset_match,
	},
};

static int __init hi3xxx_reset_init(void)
{
	return platform_driver_register(&hi3xxx_reset_driver);
}
module_init(hi3xxx_reset_init);

static void __exit hi3xxx_reset_exit(void)
{
	platform_driver_unregister(&hi3xxx_reset_driver);
}
module_exit(hi3xxx_reset_exit);

MODULE_AUTHOR("Choi Young <cuiyong1@hisilicon.com>");
MODULE_DESCRIPTION("hi3xxx reset driver");
MODULE_LICENSE("GPL v2");
