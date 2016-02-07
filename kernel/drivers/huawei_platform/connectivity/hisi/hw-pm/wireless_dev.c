

/*****************************************************************************
  1 Header File Including
*****************************************************************************/
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/clk.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#ifdef CONFIG_HWCONNECTIVITY
#include <linux/huawei/hw_connectivity.h>
#endif
#include "wireless_dev.h"
#include "wireless_pm.h"


/*****************************************************************************
  2 Global Variable Definition
*****************************************************************************/

struct dev_board_info *g_wire_dev;

/*****************************************************************************
  3 Function Definition
*****************************************************************************/

extern irqreturn_t bfg_wake_host_isr(int irq, void *dev_id); 


int hsad_hi110x_get_gpio(char *name)
{
    //int gpio = 0;

    //if (get_hw_config_int(name, &gpio, NULL ) == false)
        return -1;

    //return gpio;
}



//static void sdio_irq_gpio_down(void)
//{    
//    void __iomem *reg   = NULL;
//    int32 value         = 0;
//
//    reg = ioremap(SDIO_WAKEUP_GPIO_ADDR, ADDR_LEN);
//    value = readw(reg);
//    value = value & SDIO_WAKEUP_GPIO_DIS_UP_PULLED;
//    value = value | SDIO_WAKEUP_GPIO_EN_DOWN_PULLED;
//    writew(value, reg);
//
//    PS_PRINT_DBG("config sdio data and wakeup irq pulled down only\n");
//}



#if (PLATFORM_TYPE_FOR_HI110X == PLATFORM_HI110X_k3v2oem1 || \
             PLATFORM_TYPE_FOR_HI110X == PLATFORM_HI110X_UEDGE)
static int output_32k_clk(void)
{    
    void __iomem *reg   = NULL;
    uint8 clock         = 0;

    reg = ioremap(DEVICE_32K_B_CLOCK_ADDR, ADDR_LEN);
    clock = readw(reg);
    clock = clock | DEVICE_32K_B_CLOCK_ENABLE;
    writew(clock, reg);

    PS_PRINT_SUC("enable 32K clk for device");
}
#else
static int output_32k_clk(void)
{    
    int ret;
    
	if (NULL == g_wire_dev->clk_32kb)
	{
		PS_PRINT_SUC("enable 32K clk in NULL!!!!!!");
	}
    ret = clk_prepare_enable(g_wire_dev->clk_32kb);
    if (unlikely(ret < 0)) {
        PS_PRINT_ERR("enable clk failed");
        return DEV_FAIL;
    }

    PS_PRINT_SUC("enable 32K clk for device");
    return DEV_SUC;
}
#endif


#if (PLATFORM_TYPE_FOR_HI110X == PLATFORM_HI110X_k3v2oem1 || \
             PLATFORM_TYPE_FOR_HI110X == PLATFORM_HI110X_UEDGE)

static int set_uart_clk(void)
{
}
#else

static int set_uart_clk(void)
{    
	/*set uart4 clock to 150M*/
	int ret = DEV_SUC;
	//ret = clk_set_rate(g_wire_dev->clk_uart, DEV_UART_CLK);
#if 0
	if (DEV_SUC != ret)
	{
		PS_PRINT_ERR(" SETERROR:set uart clock err");
		ret = DEV_FAIL;
	}
#endif
	return ret;
}
#endif

/*****************************************************************************
 * Prototype    : gpio_request_and_init
 * Description  : gpio request and set the direction
 * Input        : void
 * Output       : no
 * Return       : 0 means succeed, -1 means failed
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/11/26
 *     Author       : cwx145522
 *     Modification : Created function

*****************************************************************************/
#if (PLATFORM_TYPE_FOR_HI110X == PLATFORM_HI110X_k3v2oem1 || \
             PLATFORM_TYPE_FOR_HI110X == PLATFORM_HI110X_UEDGE)
int32 gpio_request_and_init(struct platform_device *pdev)
{
    int32 ret           = 0;

    struct pm_drv_data *pm_data = platform_get_drvdata(pdev);
    if (NULL == pm_data)
    {
        PS_PRINT_ERR("pm_data is NULL!\n");
        return -1;
    }

    if ((NULL== g_wire_dev) || (INIT_OK != g_wire_dev->init_flag))
	{
        PS_PRINT_ERR("gpio get failed\n");
        return -1;
	}
    sdio_irq_gpio_down();
    output_32k_clk();

    /*request gpio and set direction(input or output)*/
    ret = gpio_request_one(HOST_WAKEUP_WLAN_GPIO, GPIOF_OUT_INIT_LOW, "host_wake_wlan");
    if (ret)
    {
        PS_PRINT_ERR("host_wake_wlan gpio_request failed\n");
        return ret;
    }

    ret = gpio_request_one(WLAN_WAKEUP_HOST_GPIO, GPIOF_IN, "wlan_wake_host");
    if (ret)
    {
        PS_PRINT_ERR("wlan_wake_host gpio_request failed\n");
        goto err_request_gpio6;
    }

    pm_data->wlan_irq = gpio_to_irq(WLAN_WAKEUP_HOST_GPIO);

    ret = gpio_request_one(BFG_WAKEUP_HOST_GPIO, GPIOF_IN, "bfg_wake_host");
    if (ret)
    {
        PS_PRINT_ERR("bfg_wake_host gpio_request failed\n");
        goto err_request_gpio5;
    }
	pm_data->bfg_irq = gpio_to_irq(BFG_WAKEUP_HOST_GPIO);

	ret = request_irq(pm_data->bfg_irq, bfg_wake_host_isr,
			IRQF_DISABLED |	IRQF_TRIGGER_RISING | IRQF_NO_SUSPEND,
				"bfg_wake_host", NULL);
    if (ret < 0)
    {
        PS_PRINT_ERR("couldn't acquire bfg_wake_host IRQ\n");
        goto err_request_gpio4;
    }

    ret = gpio_request_one(HOST_WAKEUP_BFG_GPIO, GPIOF_OUT_INIT_LOW, "host_wake_bfg");
    if (ret)
    {
        PS_PRINT_ERR("host_wake_bfg gpio_request failed\n");
        goto err_request_irq;
    }

    ret = gpio_request_one(POWER_ON_ENABLE, GPIOF_OUT_INIT_LOW, "power_on_enable");
    if (ret)
    {
        PS_PRINT_ERR("power_on_enable gpio_request failed\n");
        goto err_request_gpio3;
    }

    ret = gpio_request_one(WLAN_POWER_ENABLE, GPIOF_OUT_INIT_LOW, "wlan_power_enable");
    if (ret)
    {
        PS_PRINT_ERR("wlan_power_enable gpio_request failed\n");
        goto err_request_gpio2;
    }
    ret = gpio_request_one(BFG_POWER_ENABLE, GPIOF_OUT_INIT_LOW, "bfg_power_enable");
    if (ret)
    {
        PS_PRINT_ERR("bfg_power_enable gpio_request failed\n");
        goto err_request_gpio1;
    }

    //ret = set_uart_clk();
    //if (DEV_FAIL == ret)
    //{
    //    PS_PRINT_ERR("set uart clock failed\n");
    //    goto err_request_gpio1;
    //}


	return ret;

err_request_gpio1:
    gpio_free(WLAN_POWER_ENABLE);
err_request_gpio2:
    gpio_free(POWER_ON_ENABLE);
err_request_gpio3:
    gpio_free(HOST_WAKEUP_BFG_GPIO);
err_request_irq:
    free_irq(pm_data->bfg_irq, NULL);
err_request_gpio4:
    gpio_free(BFG_WAKEUP_HOST_GPIO);
err_request_gpio5:
    gpio_free(WLAN_WAKEUP_HOST_GPIO);
err_request_gpio6:
    gpio_free(HOST_WAKEUP_WLAN_GPIO);

    return ret;

}
#else

int32 gpio_request_and_init(struct platform_device *pdev)
{
    int32 ret           = 0;
	enum of_gpio_flags gpio_flags; 

    struct pm_drv_data *pm_data = platform_get_drvdata(pdev);
    if (NULL == pm_data)
    {
        PS_PRINT_ERR("pm_data is NULL!\n");
        return -1;
    }

    /*request gpio and set direction(input or output)*/
    ret = gpio_request_one(HOST_WAKEUP_WLAN_GPIO, GPIOF_OUT_INIT_LOW, "host_wake_wlan");
    if (ret)
    {
        PS_PRINT_ERR("host_wake_wlan gpio_request failed\n");
        return ret;
    }

    ret = gpio_request_one(WLAN_WAKEUP_HOST_GPIO, GPIOF_IN, "wlan_wake_host");
    if (ret)
    {
        PS_PRINT_ERR("wlan_wake_host gpio_request failed\n");
        goto err_request_gpio6;
    }

    pm_data->wlan_irq = gpio_to_irq(WLAN_WAKEUP_HOST_GPIO);

    ret = gpio_request_one(BFG_WAKEUP_HOST_GPIO, GPIOF_IN, "bfg_wake_host");
    if (ret)
    {
        PS_PRINT_ERR("bfg_wake_host gpio_request failed\n");
        goto err_request_gpio5;
    }
	pm_data->bfg_irq = gpio_to_irq(BFG_WAKEUP_HOST_GPIO);

	ret = request_irq(pm_data->bfg_irq, bfg_wake_host_isr,
			IRQF_DISABLED |	IRQF_TRIGGER_RISING | IRQF_NO_SUSPEND,
				"bfg_wake_host", NULL);
    if (ret < 0)
    {
        PS_PRINT_ERR("couldn't acquire bfg_wake_host IRQ\n");
        goto err_request_gpio4;
    }

    ret = gpio_request_one(HOST_WAKEUP_BFG_GPIO, GPIOF_OUT_INIT_LOW, "host_wake_bfg");
    if (ret)
    {
        PS_PRINT_ERR("host_wake_bfg gpio_request failed\n");
        goto err_request_irq;
    }

    ret = gpio_request_one(POWER_ON_ENABLE, GPIOF_OUT_INIT_LOW, "power_on_enable");
    if (ret)
    {
        PS_PRINT_ERR("power_on_enable gpio_request failed\n");
        goto err_request_gpio3;
    }

    ret = gpio_request_one(WLAN_POWER_ENABLE, GPIOF_OUT_INIT_LOW, "wlan_power_enable");
    if (ret)
    {
        PS_PRINT_ERR("wlan_power_enable gpio_request failed\n");
        goto err_request_gpio2;
    }

    ret = gpio_request_one(BFG_POWER_ENABLE, GPIOF_OUT_INIT_LOW, "bfg_power_enable");
    if (ret)
    {
        PS_PRINT_ERR("bfg_power_enable gpio_request failed\n");
        goto err_request_gpio1;
    }

    ret = set_uart_clk();
    if (DEV_FAIL == ret)
    {
        PS_PRINT_ERR("set uart clock failed\n");
        goto err_request_gpio1;
    }

    //sdio_irq_gpio_down();

	g_wire_dev->clk_32kb = devm_clk_get(&pdev->dev, g_wire_dev->clock_name);
    if (NULL == g_wire_dev->clk_32kb) 
	{
        PS_PRINT_ERR("Get bt 32k clk failed, clk=%p\n", g_wire_dev->clk_32kb);
        goto err_request_gpio1;
	}

    ret = output_32k_clk();
    if (DEV_FAIL == ret)
    {
       /* !!! not free uart clk */ 
        PS_PRINT_ERR("out put 32K clock failed\n");
        goto err_enable_clk;
    }

	return ret;

err_enable_clk:
	devm_clk_put(&pdev->dev, g_wire_dev->clk_32kb);
err_request_gpio1:
    gpio_free(WLAN_POWER_ENABLE);
err_request_gpio2:
    gpio_free(POWER_ON_ENABLE);
err_request_gpio3:
    gpio_free(HOST_WAKEUP_BFG_GPIO);
err_request_irq:
    free_irq(pm_data->bfg_irq, NULL);
err_request_gpio4:
    gpio_free(BFG_WAKEUP_HOST_GPIO);
err_request_gpio5:
    gpio_free(WLAN_WAKEUP_HOST_GPIO);
err_request_gpio6:
    gpio_free(HOST_WAKEUP_WLAN_GPIO);

    return ret;

}

#endif

static int dev_gpio_get(void)
{
	enum of_gpio_flags gpio_flags; 
    int gpio = 0;
    gpio = POWER_ON_ENABLE;
    if (gpio < 0)
    {
        PS_PRINT_ERR("hsad get hi1101 gpio power on failed");
        return -1;
    }
    g_wire_dev->power_on = POWER_ON_ENABLE;

    gpio = WLAN_POWER_ENABLE;
    if (gpio < 0)
    {
        PS_PRINT_ERR("hsad get hi1101 gpio wlan power on failed");
        return -1;
    }
    g_wire_dev->wlan_power_on = WLAN_POWER_ENABLE;

    gpio = WLAN_WAKEUP_HOST_GPIO;
    if (gpio < 0)
    {
        PS_PRINT_ERR("hsad get hi1101 gpio wlan wakeup host failed");
        return -1;
    }
    g_wire_dev->wlan_wakeup_host = WLAN_WAKEUP_HOST_GPIO;

    gpio = HOST_WAKEUP_WLAN_GPIO;
    if (gpio < 0)
    {
        PS_PRINT_ERR("hsad get hi1101 gpio host wakeup wlan failed");
        return -1;
    }
    g_wire_dev->host_wakeup_wlan = HOST_WAKEUP_WLAN_GPIO;

    gpio = BFG_POWER_ENABLE;
    if (gpio < 0)
    {
        PS_PRINT_ERR("hsad get hi1101 gpio bfg power on failed");
        return -1;
    }
    g_wire_dev->bfg_power_on = BFG_POWER_ENABLE;

    gpio = BFG_WAKEUP_HOST_GPIO;
    if (gpio < 0)
    {
        PS_PRINT_ERR("hsad get hi1101 gpio bfg wakeup host failed");
        return -1;
    }
    g_wire_dev->bfg_wakeup_host = BFG_WAKEUP_HOST_GPIO;

    gpio = HOST_WAKEUP_BFG_GPIO;
    if (gpio < 0)
    {
        PS_PRINT_ERR("hsad get hi1101 gpio host wakeup bfg failed");
        return -1;
    }
    g_wire_dev->host_wakeup_bfg = HOST_WAKEUP_BFG_GPIO;

    PS_PRINT_SUC("hi1101 gpio power_on=%d\n", g_wire_dev->power_on);
    PS_PRINT_SUC("hi1101 gpio wlan_power_on=%d\n", g_wire_dev->wlan_power_on);
    PS_PRINT_SUC("hi1101 gpio wlan_wakeup_host=%d\n", g_wire_dev->wlan_wakeup_host);
    PS_PRINT_SUC("hi1101 gpio host_wakeup_wlan=%d\n", g_wire_dev->host_wakeup_wlan);
    PS_PRINT_SUC("hi1101 gpio bfg_power_on=%d\n", g_wire_dev->bfg_power_on);
    PS_PRINT_SUC("hi1101 gpio bfg_wakeup_host=%d\n", g_wire_dev->bfg_wakeup_host);
    PS_PRINT_SUC("hi1101 gpio host_wakeup_bfg=%d\n", g_wire_dev->host_wakeup_bfg);

    return 0;

}

static int __init dev_board_init(void)
{
	int ret = -1;
	/*volatile unsigned int ret_reg = 0;*/
	struct device_node *np = NULL;
	//struct clk* clk = NULL;

#ifdef CONFIG_HWCONNECTIVITY
    if (!isMyConnectivityChip(CHIP_TYPE_HI110X )) {
        PS_PRINT_ERR("cfg dev board chip type is not match, skip driver init");
        return -EINVAL;
    } else {
        PS_PRINT_INFO("cfg dev board type is matched with hi110x, continue");
    }
#endif

	np = of_find_compatible_node(NULL, NULL, DEV_DTS_COMP_NAME);
	if (!np) {
		PS_PRINT_ERR("HISI IPC:No compatible node found.\n");
		return ret;
	}   

    g_wire_dev = (struct dev_board_info*)kzalloc(sizeof(struct dev_board_info), GFP_KERNEL);
    if (NULL == g_wire_dev)
    {
        PS_PRINT_ERR("no mem to allocate g_wire_dev\n");
        return -ENOMEM;
    }
	g_wire_dev->np = np;

	//clk = of_clk_get_by_name(np, DEV_DTS_32KB);	
	//if (NULL == clk)
	//{
	//	PS_PRINT_ERR("HISI IPC:have not get 32kb clk.\n");
    //    kfree(g_wire_dev);
	//	return ret;
	//}
	//g_wire_dev->clk_32kb = clk;

	//clk = of_clk_get_by_name(np, DEV_DTS_UART);	
	//if (NULL == clk)
	//{
	//	PS_PRINT_ERR("HISI IPC:have not get uart clk.\n");
    //    kfree(g_wire_dev);
	//	return ret;
	//}
	//g_wire_dev->clk_uart = clk;
	ret = of_property_read_string(np, "huawei,pmu_clk32b", &g_wire_dev->clock_name);
    if (ret || (NULL == g_wire_dev->clock_name)) {
        PS_PRINT_ERR("%s: read ck32b failed, ret=%d, clk_name=%p\n", __func__, ret, g_wire_dev->clock_name);
        kfree(g_wire_dev);
        return ret;
    }
	PS_PRINT_SUC("have get huawei,pmu_clk32b %s\n", g_wire_dev->clock_name);

    ret = dev_gpio_get();
    if (0 != ret)
    {
        PS_PRINT_ERR("hsad get hi1101 failed");
        kfree(g_wire_dev);
        return ret;
    }

	g_wire_dev->init_flag = INIT_OK;

#if 0
    ret_reg = hi6xxx_pmic_reg_read(0x1c);
	hi6xxx_pmic_reg_write(0x1c, (ret_reg | (1 << 6)));
#endif
    return ret;
}

device_initcall(dev_board_init);

