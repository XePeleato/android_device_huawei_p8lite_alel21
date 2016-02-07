/*!
 *****************************************************************************
 *
 * @File       sysdev.c
 * @Title      Platform-specific implementation of SYSDEV backend
 * @Description    Implementation for hisi.
 * ---------------------------------------------------------------------------
 *
 * Copyright (c) Imagination Technologies Ltd.
 * 
 * The contents of this file are subject to the MIT license as set out below.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
 * THE SOFTWARE.
 * 
 * Alternatively, the contents of this file may be used under the terms of the 
 * GNU General Public License Version 2 ("GPL")in which case the provisions of
 * GPL are applicable instead of those above. 
 * 
 * If you wish to allow use of your version of this file only under the terms 
 * of GPL, and not to allow others to use your version of this file under the 
 * terms of the MIT license, indicate your decision by deleting the provisions 
 * above and replace them with the notice and other provisions required by GPL 
 * as set out in the file called "GPLHEADER" included in this distribution. If 
 * you do not delete the provisions above, a recipient may use your version of 
 * this file under the terms of either the MIT license or GPL.
 * 
 * This License is also included in this distribution in the file called 
 * "MIT_COPYING".
 *
 *****************************************************************************/

#include <asm/io.h>
#include <asm/page.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/export.h>
#include <linux/kobject.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/clk.h>
#include <linux/regulator/consumer.h>

#include <system.h>
#include <sysmem_utils.h>
#include <sysdev_utils.h>

#define DEVICE_NAME "DT_device"

static int module_irq = -1;
static IMG_PHYSADDR module_reg_base = 0;
static unsigned int module_reg_size = 0;
static void* reg_base_addr;


/* Device information...*/
static SYSDEVU_sInfo *gpsInfo = IMG_NULL;
static bool device_detected = IMG_FALSE;

struct clk *gvenc_clk = NULL;   //j00140427
struct regulator_bulk_data gvenc_regulator = {0};

int driver_probe(struct platform_device *ofdev);

static const struct of_device_id venc_of_match[] = {
	{ .compatible = "hisi,k3v3-venc", },
	{ }
};

static struct platform_driver local_driver = {
    .probe = driver_probe,
    .driver = {
        .name = "goldfish_msvxe",
	    .owner = THIS_MODULE,
	    .of_match_table = venc_of_match
     },
};

int driver_probe(struct platform_device *ofdev) {
	int ret;
	struct resource res;

	IMG_ASSERT(ofdev->resource[0].flags == IORESOURCE_MEM);
	IMG_ASSERT(ofdev->resource[1].flags == IORESOURCE_IRQ);

	ret = of_address_to_resource(ofdev->dev.of_node, 0, &res);
	if (ret) {
		dev_err(&ofdev->dev, "missing 'reg' property in device tree\n");
		ret = -EINVAL;
		return ret;
	}
	module_reg_base = res.start;
	module_reg_size = resource_size(&res);
	reg_base_addr = of_iomap(ofdev->dev.of_node, 0);
	if (NULL == reg_base_addr) {
		printk("get reg base addr failed\n");
	}

	printk("[vxe] physical address : %llx\n", module_reg_base);

	module_irq = irq_of_parse_and_map(ofdev->dev.of_node, 0);
	if (module_irq == 0) {
		dev_err(&ofdev->dev, "could not map IRQ\n");
		module_reg_base = 0;
		module_reg_size = 0;
		ret = -EINVAL;
		return ret;
	}

	gvenc_regulator.supply = "ldo_venc";
	ret = regulator_bulk_get(&ofdev->dev, 1, &gvenc_regulator);
	if (ret) {
		printk("couldn't get regulators %d\n\r", ret);
		return -1;
	}

	gvenc_clk = of_clk_get(ofdev->dev.of_node, 0);
	if (IS_ERR(gvenc_clk))
	{
		printk("get venc clock failed\n");
		ret =  PTR_ERR(gvenc_clk);
		regulator_put(gvenc_regulator.consumer);
		memset(&gvenc_regulator,0,sizeof(gvenc_regulator));
		return -1;
	}

	gpsInfo->native_device = (void *)&ofdev->dev;

	device_detected = IMG_TRUE;
	return 0;
}

static irqreturn_t handle_interrupt(int irq, void *dev_id)
{
	bool handled;
	if ( (gpsInfo != IMG_NULL) && (gpsInfo->pfnDevKmLisr != IMG_NULL) )
	{
		SYSOSKM_DisableInt();
		handled = gpsInfo->pfnDevKmLisr(gpsInfo->pvParam);
		SYSOSKM_EnableInt();
		if (handled)
		{
			/* Signal this...*/
			return IRQ_HANDLED;
		}
	}
	printk("irq end.\n");
	return IRQ_NONE;
}

static void handle_suspend(SYSDEVU_sInfo *dev, IMG_BOOL forAPM)
{
	printk("@@@@@@@@@@@@@@@%s,%d\n",__FUNCTION__,__LINE__);

	if (gvenc_clk)
	{
		clk_disable_unprepare(gvenc_clk);
	}

	regulator_bulk_disable(1, &(gvenc_regulator));
}

static void handle_resume(SYSDEVU_sInfo *dev, IMG_BOOL forAPM)
{
	int ret = -1;

	printk("@@@@@@@@@@@@@@@%s,%d\n",__FUNCTION__,__LINE__);

	ret = regulator_bulk_enable(1, &(gvenc_regulator));
	if (ret)
		printk( "failed to enable regulators %d\n", ret);

	if (gvenc_clk)
	{
		ret = clk_prepare_enable(gvenc_clk);
		if (ret)
		{
			printk("enable clock failed\n");
			ret = -EINVAL;
		}
	}
}


static void free_device(SYSDEVU_sInfo *dev)
{
	if(!device_detected)
		return;

	iounmap(reg_base_addr);
	free_irq(module_irq, gpsInfo);
	platform_driver_unregister(&local_driver);

	if (NULL != gvenc_clk) {
		clk_put( gvenc_clk);
		gvenc_clk = NULL;
	}

	regulator_put(gvenc_regulator.consumer);
	memset(&gvenc_regulator,0,sizeof(gvenc_regulator));
}

static struct SYSDEV_ops device_ops = {
		.free_device = free_device,

		.resume_device = handle_resume,
		.suspend_device = handle_suspend
};

IMG_RESULT SYSDEVU_RegisterDriver(SYSDEVU_sInfo *sysdev) {
	int ret = 0;

	gpsInfo = sysdev;

	ret = platform_driver_register(&local_driver);
	if (ret != 0) {
		ret = IMG_ERROR_DEVICE_NOT_FOUND;
		goto failure_register;
	}

	if (device_detected != IMG_TRUE) {
		ret = IMG_ERROR_DEVICE_NOT_FOUND;
		goto failure_detect;
	}

	if (request_irq(module_irq, handle_interrupt, 0, DEVICE_NAME, gpsInfo)) {
		printk(KERN_ERR "goldfish_device: Failed to get IRQ\n");
		ret = IMG_ERROR_GENERIC_FAILURE;
		goto failure_request_irq;
	}

	SYSDEVU_SetDevMap(sysdev, module_reg_base, ((IMG_UINT32 *)reg_base_addr), module_reg_size, 0, IMG_NULL, 0, 0);
	SYSDEVU_SetDeviceOps(sysdev, &device_ops);

	ret = SYSMEMKM_AddSystemMemory(sysdev, &sysdev->sMemPool);
	if(IMG_SUCCESS != ret)
	{
		goto failure_add_sys_mem;
	}

	/* Return success...*/
	return IMG_SUCCESS;

failure_add_sys_mem:
failure_request_irq:
	iounmap(reg_base_addr);
failure_detect:
	platform_driver_unregister(&local_driver);
failure_register:
	gpsInfo = IMG_NULL;
	return ret;
}

IMG_RESULT SYSDEVU_UnRegisterDriver(SYSDEVU_sInfo *sysdev) {
	SYSMEMU_RemoveMemoryHeap(sysdev->sMemPool);
	sysdev->ops->free_device(sysdev);
	return IMG_SUCCESS;
}
