#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/pinctrl/consumer.h>
#include <linux/gpio.h>
#include <linux/kernel.h>
#include <huawei_platform/log/hw_log.h>
#include <linux/slab.h>

#define IRDA_COMPATIBLE_ID		"huawei,irda_maxim"
#define IRDA_POWER_CONTROL_GPIO		"gpio_power_control"
#define IRDA_BUFF_SIZE			50
#define HWLOG_TAG irda
HWLOG_REGIST();

struct irda_private_data {
	struct gpio gpio_reset;
};

static ssize_t power_config_get(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct irda_private_data *pdata = dev_get_drvdata(dev);
	return snprintf(buf, IRDA_BUFF_SIZE, "%d\n", gpio_get_value(pdata->gpio_reset.gpio));
}

static ssize_t power_config_set(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int ret = -1;
	unsigned long state = 0;

	struct irda_private_data *pdata = dev_get_drvdata(dev);
	if (!strict_strtol(buf, 10, &state)){
		gpio_direction_output(pdata->gpio_reset.gpio, !!state);
		ret = count;
	}
	return ret;
}

static DEVICE_ATTR(power_cfg, 0660, power_config_get, power_config_set);

static int irda_probe(struct platform_device *pdev)
{
	int ret;
	struct device_node *np;
	struct irda_private_data *pdata;

	np= pdev->dev.of_node;
	if (np ==NULL) {
		hwlog_err("none device\n");
		ret = -ENODEV;
		goto error;
	}

	pdata = kzalloc(sizeof(struct irda_private_data), GFP_KERNEL);
	if (NULL == pdata) {
		hwlog_err("failed to allocate irda_private_data\n");
		ret = -ENOMEM;
		goto error;
	}

	ret = device_rename(&pdev->dev, "irda_maxim");
	if (ret) {
		hwlog_err("dev rename err\n");
		goto free_data;
	}

	pdata->gpio_reset.gpio= of_get_named_gpio(np, IRDA_POWER_CONTROL_GPIO, 0);
	if (!gpio_is_valid(pdata->gpio_reset.gpio)) {
		hwlog_err("gpio is not valid\n");
		ret =  -EINVAL;
		goto free_data;
	}

	ret = gpio_request(pdata->gpio_reset.gpio, "irda_gpio");
	if (ret) {
		hwlog_err("gpio[%ud] request faile", pdata->gpio_reset.gpio);
		goto free_data;
	}

	ret = device_create_file(&pdev->dev, &dev_attr_power_cfg);
	if (ret) {
		hwlog_err("create file err !\n");
		goto free_gpio;
	}

	gpio_direction_output(pdata->gpio_reset.gpio, 0);
	platform_set_drvdata(pdev, pdata);

	hwlog_info("platform device probe success\n");
	return 0;

free_gpio:
	gpio_free(pdata->gpio_reset.gpio);
free_data:
	kfree(pdata);
error:
	return ret;
}

static int irda_remove(struct platform_device *pdev)
{
	struct irda_private_data *pdata = platform_get_drvdata(pdev);
	device_remove_file(&pdev->dev, &dev_attr_power_cfg);
	gpio_free(pdata->gpio_reset.gpio);
	kfree(pdata);

	return 0;
}

static const struct of_device_id irda_match_table [] = {
		{
			.compatible = IRDA_COMPATIBLE_ID,
			.data = NULL,
		},
		{
		},
};

MODULE_DEVICE_TABLE(of, irda_match_table);

static struct platform_driver irda_driver = {
	.probe = irda_probe,
	.remove = irda_remove,
	.driver = {
		.name = "irda_maxq616",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(irda_match_table),
	},
};

static int __init irda_remote_init(void)
{
	return platform_driver_register(&irda_driver);
}

static void __exit irda_remote_exit(void)
{
	platform_driver_unregister(&irda_driver);
}

module_init(irda_remote_init);
module_exit(irda_remote_exit);

MODULE_AUTHOR("Huawei");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("irda power control driver");
