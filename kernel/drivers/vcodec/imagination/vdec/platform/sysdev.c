/*!
 *****************************************************************************
 *
 * @File       sysdev.c
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

#include <sysdev_utils.h>
#include <linux/kobject.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <asm/page.h>
#include <linux/mm.h>
#include <linux/version.h>
#include <linux/module.h>
#include <img_defs.h>
#include "target.h"
#include <sysenv_api_km.h>
#include <system.h>
#include <sysmem_utils.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,2,0)
#include <linux/export.h>
#endif
#include <linux/clk.h>
#include <linux/regulator/consumer.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>

static int module_irq = -1;

void *vdec_reg_vaddr;
IMG_PHYSADDR vdec_reg_paddr = 0;
unsigned int vdec_reg_size = 0;

//Device information
static IMG_CHAR *   gpszDevName = IMG_NULL;
static SYSDEVU_sInfo *  psSysDev = IMG_NULL;

//struct clk *gvdec_clk = NULL;   //j00140427
struct regulator_bulk_data gvdec_regulator = {0};
static IMG_BOOL gbDevDetected = IMG_FALSE;

static int driver_probe(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
    struct device_node *np = dev->of_node;
    struct resource res;
    int ret = 0;
    IMG_ASSERT(pdev->resource[0].flags == IORESOURCE_MEM);
    IMG_ASSERT(pdev->resource[1].flags == IORESOURCE_IRQ);
    printk("VDEC %s   %d   \n", __FUNCTION__, __LINE__);
    if (NULL == np)
    {
        printk("the device node is null\n");
        return -1;
    }
    printk("VDEC %s   %d   \n", __FUNCTION__, __LINE__);
    module_irq =  irq_of_parse_and_map(np,0);
    printk("%s, %d,module_irq = %d\n",__func__,__LINE__,module_irq);

    ret = of_address_to_resource(np,0,&res);
    if (ret == 0)
    {
        vdec_reg_paddr = res.start;
        vdec_reg_size = resource_size(&res);
        printk("%s, %d,vdec_reg_vaddr = %#llx,vdec_reg_size = %#x\n", __func__, __LINE__, vdec_reg_paddr, vdec_reg_size);
    }   

    vdec_reg_vaddr = of_iomap(np, 0);
    if (NULL == vdec_reg_vaddr) {
        printk("get reg base addr failed\n");
    }

    gvdec_regulator.supply = "ldo_vdec";
    ret = regulator_bulk_get(dev, 1, &gvdec_regulator);
    if (ret) {
        printk("couldn't get regulators %d\n\r", ret);
        return -1;
    }

    //gvdec_clk = of_clk_get(np,0);
    /*if (IS_ERR(gvdec_clk))
    {
        printk("get venc clock failed\n");
        ret =  PTR_ERR(gvdec_clk);
        regulator_put(gvdec_regulator.consumer);
        memset(&gvdec_regulator,0,sizeof(gvdec_regulator));
        return -1;
    }*/

    psSysDev->native_device = (void *)&pdev->dev;

    gbDevDetected = IMG_TRUE;
    return 0;
}

static const struct of_device_id vdec_of_match[] = {
    { .compatible = "hisi,k3v3-vdec", },
    { }
};

static struct platform_driver local_driver = {
    .probe = driver_probe,
    .driver = {
        .name = "goldfish_msvxd", 
        .owner = THIS_MODULE,
        .of_match_table = vdec_of_match
     },
};

/*!
******************************************************************************

@Function IsrCb

******************************************************************************/
static irqreturn_t IsrCb(int irq, void *dev_id)
{
    IMG_BOOL  bHandled;

    if ( (psSysDev != IMG_NULL) && (psSysDev->pfnDevKmLisr != IMG_NULL) )
    {
        //Call it
        SYSOSKM_DisableInt();
        bHandled = psSysDev->pfnDevKmLisr(psSysDev->pvParam);
        SYSOSKM_EnableInt();

        //If the LISR handled the interrupt
        if (bHandled)
        {
            //Signal this
            return IRQ_HANDLED;
        }
    }

    return IRQ_NONE;
}


/*!
******************************************************************************

@Function free_device

******************************************************************************/
static IMG_VOID free_device(SYSDEVU_sInfo *psInfo)
{
    if (IMG_TRUE == gbDevDetected)
    {
        platform_driver_unregister(&local_driver);
        gbDevDetected = IMG_FALSE;
    }
    /*if (NULL != gvdec_clk) {
        clk_put( gvdec_clk);
        gvdec_clk = NULL;
    }*/

    regulator_put(gvdec_regulator.consumer);
    memset(&gvdec_regulator,0,sizeof(gvdec_regulator));

    if(psSysDev != psInfo)
    {
        printk(KERN_ERR "VxD: Failed to free the Device\n");
        return;
    }

    //Free device name
    if (gpszDevName != IMG_NULL)
    {
        IMG_FREE(gpszDevName);
        gpszDevName = IMG_NULL;
    }

    if(IMG_NULL != psSysDev)
    {
        free_irq(module_irq, psSysDev);
    }

    psSysDev = IMG_NULL;

    return;
}


/*!
******************************************************************************

@Function handleResume

******************************************************************************/
static IMG_VOID handle_resume(SYSDEVU_sInfo *psInfo, IMG_BOOL forAPM)
{
    int ret = -1;    

    // printk removed for performace reasons.
    printk("VXD %s,%d\n",__FUNCTION__,__LINE__);

    ret = regulator_bulk_enable(1, &(gvdec_regulator));
    if (ret)
        printk( "failed to enable regulators %d\n", ret);

    /*if (gvdec_clk)
    {
        ret = clk_prepare_enable(gvdec_clk);
        if (ret)
        {
            printk("enable clock failed\n");
            ret = -EINVAL;
        }
    }*/
}


/*!
******************************************************************************

@Function handleSuspend

******************************************************************************/
static IMG_VOID handle_suspend(SYSDEVU_sInfo *psInfo, IMG_BOOL forAPM)
{
    // printk removed for performace reasons.
    printk("VXD %s,%d\n",__FUNCTION__,__LINE__);

    /*if (gvdec_clk)
    {
        clk_disable_unprepare(gvdec_clk);
    }*/

    regulator_bulk_disable(1, &(gvdec_regulator));

}

static struct SYSDEV_ops device_ops = {
		.free_device = free_device,

		.resume_device = handle_resume,
		.suspend_device = handle_suspend
};

/*!
******************************************************************************

@Function    SYSDEVU_VDECRegisterDriver

******************************************************************************/
IMG_RESULT SYSDEVU_VDECRegisterDriver(
    SYSDEVU_sInfo *  psInfo
)
{
    IMG_UINT32 ui32Result = IMG_ERROR_GENERIC_FAILURE;

    if(IMG_FALSE == gbDevDetected)
    {
        printk(KERN_INFO "SYSDEVU1_Initialise\n");

        psSysDev = psInfo;

		ui32Result = platform_driver_register(&local_driver);
		if (ui32Result != 0) 
		{
			return IMG_ERROR_DEVICE_NOT_FOUND;
		}
        if (request_irq(module_irq, IsrCb, 0, "goldfish_device", psSysDev)) 
        {
            printk(KERN_ERR "Goldfish: Failed to get IRQ\n");
            return IMG_ERROR_GENERIC_FAILURE;
        }

        gbDevDetected = IMG_TRUE;

        SYSDEVU_SetDevMap(psInfo, 0, 0, 0, 0, 0, 0, 0);

        SYSDEVU_SetDeviceOps(psInfo, &device_ops);

        ui32Result = SYSMEMKM_AddSystemMemory(psInfo, &psInfo->sMemPool);
    }

    return ui32Result;
}
/*!
******************************************************************************

@Function    SYSDEVU_VDECUnRegisterDriver

******************************************************************************/
IMG_RESULT SYSDEVU_VDECUnRegisterDriver(
    SYSDEVU_sInfo *  psInfo
)
{
    SYSMEMU_RemoveMemoryHeap(psInfo->sMemPool);
    psInfo->ops->free_device(psInfo);
    return IMG_SUCCESS;
}
