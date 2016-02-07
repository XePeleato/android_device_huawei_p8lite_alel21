#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/kernel.h>

#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_irq.h>
#include <linux/of_device.h>
#include <linux/of_address.h>
#ifdef CONFIG_HISI_RDR
#include <linux/huawei/rdr.h>
#endif

#define MODULE_NAME	"hisilicon,extern_wdt"
#define WDTCONTROL		0x008
#define WDTLOCK			0xC00
	#define	UNLOCK		0x1ACCE551
	#define	LOCK		0x00000001

enum extern_wdt_type {
	IOM3_WDT = 0,
	LPM3_WDT,
	HIFI_WDT,
	MODEM_WDT,
	EXTERN_WDT_NR
};
#ifdef CONFIG_HISI_REBOOT_TYPE
extern void set_watchdog_resetflag(void);
#endif
extern void emergency_restart(void);

struct hisi_extern_wdt {
	spinlock_t		lock;
	unsigned int	extern_wdt_irq[EXTERN_WDT_NR];
	void __iomem	*extern_wdt_reg[EXTERN_WDT_NR];
	struct platform_device *pdev;
};

static irqreturn_t hisi_wdt_iom3_interrupt(int irq, void *dev_id)
{
	struct platform_device *pdev = (struct platform_device *)dev_id;
	struct device *dev = &pdev->dev;

	dev_warn(dev, "%s start!\n", __func__);

	#ifdef CONFIG_HISI_RDR
	hisi_system_error(HISI_RDR_MOD_IOM3_WD, 0, 0, 0, 0);
	#else
	#ifdef CONFIG_HISI_REBOOT_TYPE
	/* set flag for reset_type*/
	set_watchdog_resetflag();
	#endif
	emergency_restart();
	#endif

	return IRQ_HANDLED;
}

static irqreturn_t hisi_wdt_lpm3_interrupt(int irq, void *dev_id)
{
	struct platform_device *pdev = (struct platform_device *)dev_id;
	struct device *dev = &pdev->dev;

	dev_warn(dev, "%s start!\n", __func__);

	#ifdef CONFIG_HISI_RDR
	hisi_system_error(HISI_RDR_MOD_LPM3_WD, 0, 0, 0, 0);
	#else
	#ifdef CONFIG_HISI_REBOOT_TYPE
	/* set flag for reset_type*/
	set_watchdog_resetflag();
	#endif
	emergency_restart();
	#endif

	return IRQ_HANDLED;
}

static irqreturn_t hisi_wdt_hifi_interrupt(int irq, void *dev_id)
{
	void __iomem *base = NULL;
	struct platform_device *pdev = (struct platform_device *)dev_id;
	struct device *dev = &pdev->dev;
	struct hisi_extern_wdt *wdt = (struct hisi_extern_wdt *)platform_get_drvdata(pdev);
	if (wdt == NULL) {
		pr_err("%s %d have NULL pointer\n", __func__, __LINE__);
		return -1;
	}

	base = wdt->extern_wdt_reg[HIFI_WDT];

	dev_warn(dev, "%s start!\n", __func__);

	spin_lock(&wdt->lock);

	writel_relaxed(UNLOCK, base + WDTLOCK);
	writel_relaxed(0, base + WDTCONTROL);
	writel_relaxed(LOCK, base + WDTLOCK);

	/* Flush posted writes. */
	readl_relaxed(base + WDTLOCK);
	spin_unlock(&wdt->lock);

	#ifdef CONFIG_HISI_RDR
	hisi_system_error(HISI_RDR_MOD_HIFI_WD, 0, 0, 0, 0);
	#endif

	return IRQ_HANDLED;
}

static irqreturn_t hisi_wdt_modem_interrupt(int irq, void *dev_id)
{
	struct platform_device *pdev = (struct platform_device *)dev_id;
	struct device *dev = &pdev->dev;

	dev_warn(dev, "%s start!\n", __func__);

	#ifdef CONFIG_HISI_RDR
	hisi_system_error(HISI_RDR_MOD_CP_WD, 0, 0, 0, 0);
	#else
	#ifdef CONFIG_HISI_REBOOT_TYPE
	/* set flag for reset_type*/
	set_watchdog_resetflag();
	#endif
	emergency_restart();
	#endif

	return IRQ_HANDLED;
}

static int hisi_extern_wdt_probe(struct platform_device *pdev)
{
	struct device_node *extern_wdt_irq_np;
	struct hisi_extern_wdt *wdt;
	struct device *dev = &pdev->dev;
	int ret = 0;

	wdt = devm_kzalloc(dev, sizeof(*wdt), GFP_KERNEL);
	if (!wdt) {
		dev_warn(dev, "kzalloc failed\n");
		ret = -ENOMEM;
		goto err;
	}

	extern_wdt_irq_np = of_find_compatible_node(NULL, NULL, "hisilicon,extern_wdt");
	if (IS_ERR(extern_wdt_irq_np)) {
		dev_err(dev, "can not find extern wdt node\n");
		goto err;
	}

	wdt->extern_wdt_reg[HIFI_WDT] = of_iomap(extern_wdt_irq_np, 0);
	if (wdt->extern_wdt_reg[HIFI_WDT] == NULL) {
		dev_err(dev, "iomap hifi reg failed!\n");
		goto err;
	}

	spin_lock_init(&wdt->lock);

	wdt->extern_wdt_irq[IOM3_WDT] = irq_of_parse_and_map(extern_wdt_irq_np, 0);
	if (devm_request_irq(dev, wdt->extern_wdt_irq[IOM3_WDT], hisi_wdt_iom3_interrupt,
		IRQF_TRIGGER_RISING | IRQF_NO_SUSPEND, MODULE_NAME, pdev)) {
		ret = -EIO;
		dev_err(dev, "hisi_wdt_iom3_interrupt request failed! irq = %d\n", wdt->extern_wdt_irq[IOM3_WDT]);
		goto err;
	}

	wdt->extern_wdt_irq[LPM3_WDT] = irq_of_parse_and_map(extern_wdt_irq_np, 1);
	if (devm_request_irq(dev, wdt->extern_wdt_irq[LPM3_WDT], hisi_wdt_lpm3_interrupt,
		IRQF_TRIGGER_RISING | IRQF_NO_SUSPEND, MODULE_NAME, pdev)) {
		ret = -EIO;
		dev_err(dev, "hisi_wdt_lpm3_interrupt request failed! irq = %d\n", wdt->extern_wdt_irq[LPM3_WDT]);
		goto err;
	}

	wdt->extern_wdt_irq[HIFI_WDT] = irq_of_parse_and_map(extern_wdt_irq_np, 2);
	if (devm_request_irq(dev, wdt->extern_wdt_irq[HIFI_WDT], hisi_wdt_hifi_interrupt,
		IRQF_TRIGGER_RISING | IRQF_NO_SUSPEND, MODULE_NAME, pdev)) {
		ret = -EIO;
		dev_err(dev, "hisi_wdt_hifi_interrupt request failed! irq = %d\n", wdt->extern_wdt_irq[HIFI_WDT]);
		goto err;
	}

	wdt->extern_wdt_irq[MODEM_WDT] = irq_of_parse_and_map(extern_wdt_irq_np, 3);
	if (devm_request_irq(dev, wdt->extern_wdt_irq[MODEM_WDT], hisi_wdt_modem_interrupt,
		IRQF_TRIGGER_RISING | IRQF_NO_SUSPEND, MODULE_NAME, pdev)) {
		ret = -EIO;
		dev_err(dev, "hisi_wdt_modem_interrupt request failed! irq = %d\n", wdt->extern_wdt_irq[MODEM_WDT]);
		goto err;
	}

	wdt->pdev = pdev;
	platform_set_drvdata(pdev, wdt);

	dev_info(dev, "%s successful!\n", __func__);
	return 0;

err:
	dev_err(dev, "%s failed!!!\n", __func__);
	return ret;
}

static int hisi_extern_wdt_remove(struct platform_device *pdev)
{
	dev_info(&pdev->dev, "%s successful!\n", __func__);
	return 0;
}

static const struct of_device_id extern_wdt_match[] = {
	{ .compatible = MODULE_NAME },
	{},
};
MODULE_DEVICE_TABLE(of, extern_wdt_match);

static struct platform_driver hisi_extern_wdt_driver = {
	.driver = {
		.name = MODULE_NAME,
		.of_match_table = of_match_ptr(extern_wdt_match),
	},
	.probe = hisi_extern_wdt_probe,
	.remove = hisi_extern_wdt_remove,
};

static int __init hisi_extern_wdt_init(void)
{
	return platform_driver_register(&hisi_extern_wdt_driver);
}
module_init(hisi_extern_wdt_init);

static void __exit hisi_extern_wdt_exit(void)
{
	platform_driver_unregister(&hisi_extern_wdt_driver);
}
module_exit(hisi_extern_wdt_exit);

MODULE_DESCRIPTION("EXTERN WATCHDOG DRIVER");
MODULE_LICENSE("GPL");
