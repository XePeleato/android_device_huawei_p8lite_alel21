/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : drv_if_misc.c
  版 本 号   : 初稿
  作    者   : 苏庄銮 59026
  生成日期   : 2012年9月20日
  最近修改   :
  功能描述   : 与NAS其它接口实现
  函数列表   :
  修改历史   :
  1.日    期   : 2012年9月20日
    作    者   : 苏庄銮 59026
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "BSP.h"
#include <linux/hisi/drv_misc_if.h>
#include <linux/gpio.h>
#include <linux/module.h>


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


/*****************************************************************************
  3 函数实现
*****************************************************************************/

/*****************************************************************************
* 函 数 名  : BSP_WDT_HardwareFeed
*
* 功能描述  : 重置看门狗计数寄存器（喂狗）
*
* 输入参数  : wdtId  看门狗ID
*
* 输出参数  : 无
*
* 返 回 值  : OK& ERROR
*
* 修改记录 :  2009年3月5日  v1.00  wangxuesong  创建
*****************************************************************************/
BSP_S32 BSP_WDT_HardwareFeed(BSP_U8 wdtId)
{
    return BSP_OK;
}



unsigned int  platform_have_bbp(void)
{
#if 0 /*y00145322*/
#if defined(BOARD_ASIC)
    return 0;   /* V9R1回片不存在该拨码,返回0,起协议栈 */
#else
    static int request_flag = 0;
    int ret = 0;
    int have_bbp_gpio;
	
    //ret = get_hw_config_int("bbp/have_bbp_gpio", &have_bbp_gpio, NULL);
    if (false == ret) {
        printk("have_bbp_gpio value not found !\n");
        have_bbp_gpio = GPIO_0_0;
    } 

    if (request_flag == 0) {
        ret = gpio_request(have_bbp_gpio, "have_bbp");
        if (ret) {
            printk("%s: platform have bbp gpio_request failed\n", __func__);
            goto ERR;
        }

        ret = gpio_direction_input(have_bbp_gpio);
        if(ret) {
            printk("%s: platform have bbp set direction err\n", __func__);
            goto ERR;
        }

        request_flag = 1;
    }
    
    ret = gpio_get_value(have_bbp_gpio);

    /*拨码确定:1:起协议栈 0:不起   */
    if ( PLATFORM_NO_BBP != ret )
    {
        return 0;
    }

ERR:
    return DRV_NOT_START_UMTS;
#endif
#endif
    return 0;
}



void BSP_TRACE(BSP_LOG_LEVEL_E enLogLevel, BSP_U32 enLogModule, const char *fmt, ...)
{
    return;
}

int device_event_report(void *data, int len)
{
    return 0;
}

EXPORT_SYMBOL(platform_have_bbp);
EXPORT_SYMBOL(BSP_TRACE);
EXPORT_SYMBOL(device_event_report);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif




