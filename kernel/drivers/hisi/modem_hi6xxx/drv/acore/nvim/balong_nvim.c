#include <BSP.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include "balong_nvim.h"
#include <linux/semaphore.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <platdrv.h>

MPS_PROC_S om_msp_hooks = {NULL};

/*****************************************************************************
* 函 数 名  : DR_NV_Read
*
* 功能描述  : A核读NV接口函数
*
* 输入参数  : usID -> NV ID
              *pItem -> Save NV data buffer
              ulLength -> buffer length
* 输出参数  : 无
*
* 返 回 值  : NV_OK:    操作成功
*             NV_ERROR: 操作失败
*
* 其它说明  : 无
*
*****************************************************************************/
int DR_NV_Read(unsigned short usID,void *pItem,unsigned int ulLength)
{
    int ret = NV_OK;
    if (NULL == om_msp_hooks.OM_read_nv)
    {
        printk( KERN_ERR " Kernel NV read function have NOT been configured!!! ");
        return NV_ERROR;
    }
    else
    {
        printk( KERN_INFO " Kernel NV read function have been configured!!! ");

        ret = ( *om_msp_hooks.OM_read_nv )(0,usID,pItem,ulLength);
    }
    return ret;
}
EXPORT_SYMBOL(DR_NV_Read);


/*****************************************************************************
* 函 数 名  : DR_NV_Write
*
* 功能描述  : A核写NV接口函数
*
* 输入参数  : usID -> NV ID
              *pItem -> Save NV data buffer
              ulLength -> buffer length
* 输出参数  : 无
*
* 返 回 值  : NV_OK:    操作成功
*             NV_ERROR: 操作失败
*
* 其它说明  : 无
*
*****************************************************************************/
int DR_NV_Write(unsigned short usID,void *pItem,unsigned int ulLength)
{
    int ret = NV_OK;
    if (NULL == om_msp_hooks.OM_write_nv)
    {
        printk(KERN_ERR " Kernel NV write function have NOT been configured!!! ");
        return NV_ERROR;
    }
    else
    {
        printk(KERN_INFO " Kernel NV write function have been configured!!! ");

        ret = (*om_msp_hooks.OM_write_nv)(0,usID,pItem,ulLength);
    }
    return ret;
}
EXPORT_SYMBOL(DR_NV_Write);

