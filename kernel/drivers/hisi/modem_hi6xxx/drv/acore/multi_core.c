#include <linux/init.h>
#include "BSP.h"
#include <linux/module.h>
#include "soc_baseaddr_interface.h"
#include "soc_ao_sctrl_interface.h"
#include "product_config.h"
#include <linux/hisi/hi6xxx-iomap.h>
#include <mdrv_udi.h>

int g_cshell_udi_handle = -1;
extern unsigned int cshell_event_cb(unsigned int id, unsigned int event, void *Param);
extern unsigned int cshell_read_cb(unsigned int id, int size);
extern unsigned int cshell_write_cb(unsigned int id);
extern void NV_ACoreInitSync(void);

extern int check_acore_only(void);

void cshell_icc_open(void)
{
    ICC_CHAN_ATTR_S attr;
    UDI_OPEN_PARAM_S cshell_icc_param;
    UDI_HANDLE cshell_udi_handle = UDI_INVALID_HANDLE;

    attr.enChanMode  = ICC_CHAN_MODE_STREAM;
    attr.u32Priority = 255;
    attr.u32TimeOut  = 1000;
    attr.event_cb = cshell_event_cb;
    attr.read_cb  = cshell_read_cb;
    attr.write_cb = cshell_write_cb;
    attr.u32FIFOInSize  = 8192;
    attr.u32FIFOOutSize = 8192;

    cshell_icc_param.devid = (UDI_DEVICE_ID_E)UDI_BUILD_DEV_ID(UDI_DEV_ICC, 31);
    cshell_icc_param.pPrivate = &attr;

    cshell_udi_handle = mdrv_udi_open(&cshell_icc_param);
    if(cshell_udi_handle <= 0)
    {
        printk("cshell_icc_open fail,cshell_udi_handle is %d \n",cshell_udi_handle);
        g_cshell_udi_handle = -1;
    }
    else
    {
        printk("cshell_icc_open success,cshell_udi_handle is %d \n",cshell_udi_handle);
        g_cshell_udi_handle = cshell_udi_handle;
    }
}
UDI_HANDLE cshell_get_handle(void)
{
    return g_cshell_udi_handle;
}
EXPORT_SYMBOL(cshell_get_handle);
static int __init multicore_init(void)
{
    unsigned int ret = 0;

    printk(KERN_INFO"multicore_init start\n");

    BSP_UDI_Init();

    printk(KERN_INFO"multicore_init end\n");

    /*added by zhuzheng for v8r2*/
    if (1 == check_acore_only())
        return ret;

    printk(KERN_INFO"NV_ACoreInitSync begin.\n");
    NV_ACoreInitSync();
    printk(KERN_INFO"NV_ACoreInitSync end.\n");
    return ret;
}

arch_initcall(multicore_init);


/*共用UART0的标志设置*/
void set_uart0_master(char flag)
{
	/*在系统保留寄存器中存上标志，供MCU/CCPU使用*/
	*(volatile int *)HISI_VA_ADDRESS(SOC_AO_SCTRL_SC_RESERVED31_ADDR(SOC_AO_SCTRL_BASE_ADDR)) = (int)flag;
}

void switchA(void)
{
    set_uart0_master('A');
}

void switchC(void)
{
    set_uart0_master('C');
}

void switchM(void)
{
    set_uart0_master('M');
}


EXPORT_SYMBOL(switchA);
EXPORT_SYMBOL(switchC);
EXPORT_SYMBOL(switchM);

