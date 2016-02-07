

#ifndef __WIRELESS_DEV_H__
#define __WIRELESS_DEV_H__
/*****************************************************************************
  1 Include other Head file
*****************************************************************************/

#include "../hw-ps/hw_bfg_ps.h"
#include <linux/mfd/hisi_hi6xxx_pmic.h>
/*****************************************************************************
  2 Define macro
*****************************************************************************/

#if (PLATFORM_TYPE_FOR_HI110X == PLATFORM_HI110X_k3v2oem1)

#define WLAN_POWER_ENABLE               GPIO_0_2
#define WLAN_WAKEUP_HOST_GPIO           GPIO_0_4
#define HOST_WAKEUP_WLAN_GPIO           GPIO_19_0 /* previous GPIO_21_0 , Chenwanjin 2013/6/6 */
#define BFG_WAKEUP_HOST_GPIO            GPIO_20_6
#define HOST_WAKEUP_BFG_GPIO            GPIO_20_7
#define POWER_ON_ENABLE                 GPIO_19_1
#define BFG_POWER_ENABLE                GPIO_21_1

#elif (PLATFORM_TYPE_FOR_HI110X == PLATFORM_HI110X_UEDGE)

#define WLAN_POWER_ENABLE               GPIO_0_2
#define WLAN_WAKEUP_HOST_GPIO           GPIO_0_4
#define HOST_WAKEUP_WLAN_GPIO           GPIO_19_0 /* previous GPIO_21_0 , Chenwanjin 2013/6/6 */
#define BFG_WAKEUP_HOST_GPIO            GPIO_20_6
#define HOST_WAKEUP_BFG_GPIO            GPIO_20_7
//#define POWER_ON_ENABLE                 GPIO_19_1
#define POWER_ON_ENABLE                 GPIO_18_7 /*modify for p6-u 2013年 10月 17日 星期四 14:31:31 CST */
#define BFG_POWER_ENABLE                GPIO_21_1

//#elif (TARGET_PRODUCT == OSCAR_3G)
//#elif (TARGET_PRODUCT == hi6620oem)
#else
//#include <mach/clock.h>
//#include <mach/clk_name_interface.h>
//#if 1
//#ifdef  WLAN_GPIO_SET
//#define POWER_ON_ENABLE                 GPIO_17_0
//#define WLAN_WAKEUP_HOST_GPIO           GPIO_0_4
//#define HOST_WAKEUP_BFG_GPIO            GPIO_19_1
//#else
//#define POWER_ON_ENABLE                 GPIO_1_7
//#define HOST_WAKEUP_BFG_GPIO            GPIO_0_4
//#define WLAN_WAKEUP_HOST_GPIO           GPIO_19_1
//
//#endif
//
//#define WLAN_POWER_ENABLE               GPIO_17_2
//#define HOST_WAKEUP_WLAN_GPIO           GPIO_17_3 /* previous GPIO_21_0 , Chenwanjin 2013/6/6 */
//#define BFG_WAKEUP_HOST_GPIO            GPIO_5_1
//#define BFG_POWER_ENABLE                GPIO_1_4
//#endif
#define POWER_ON_ENABLE				  			of_get_gpio_by_prop(g_wire_dev->np, "hi1101,gpio_power_on" , 0 , 0 , &gpio_flags)

#define WLAN_POWER_ENABLE             of_get_gpio_by_prop(g_wire_dev->np, "hi1101,gpio_wlan_power_on", 0, 0 , &gpio_flags)
#define WLAN_WAKEUP_HOST_GPIO         of_get_gpio_by_prop(g_wire_dev->np, "hi1101,gpio_wlan_wakeup_host", 0, 0 , &gpio_flags)
#define HOST_WAKEUP_WLAN_GPIO         of_get_gpio_by_prop(g_wire_dev->np, "hi1101,gpio_host_wakeup_wlan", 0, 0 , &gpio_flags)

#define BFG_POWER_ENABLE              of_get_gpio_by_prop(g_wire_dev->np, "hi1101,gpio_bfg_power_on", 0, 0 , &gpio_flags)
#define BFG_WAKEUP_HOST_GPIO          of_get_gpio_by_prop(g_wire_dev->np, "hi1101,gpio_bfg_wakeup_host", 0, 0 , &gpio_flags)
#define HOST_WAKEUP_BFG_GPIO          of_get_gpio_by_prop(g_wire_dev->np, "hi1101,gpio_host_wakeup_bfg", 0, 0 , &gpio_flags)

#if 0
#define POWER_ON_ENABLE               of_get_named_gpio(g_wire_dev->np, "hi1101,gpio_power_on", 0) 

#define WLAN_POWER_ENABLE             of_get_named_gpio(g_wire_dev->np, "hi1101,gpio_wlan_power_on", 0)
#define WLAN_WAKEUP_HOST_GPIO         of_get_named_gpio(g_wire_dev->np, "hi1101,gpio_wlan_wakeup_host", 0)
#define HOST_WAKEUP_WLAN_GPIO         of_get_named_gpio(g_wire_dev->np, "hi1101,gpio_host_wakeup_wlan", 0)

#define BFG_POWER_ENABLE              of_get_named_gpio(g_wire_dev->np, "hi1101,gpio_bfg_power_on", 0)
#define BFG_WAKEUP_HOST_GPIO          of_get_named_gpio(g_wire_dev->np, "hi1101,gpio_bfg_wakeup_host", 0)
#define HOST_WAKEUP_BFG_GPIO          of_get_named_gpio(g_wire_dev->np, "hi1101,gpio_host_wakeup_bfg", 0)
#endif

#endif


#if (PLATFORM_TYPE_FOR_HI110X == PLATFORM_HI110X_k3v2oem1 || \
     PLATFORM_TYPE_FOR_HI110X == PLATFORM_HI110X_UEDGE)

#define DEVICE_32K_B_CLOCK_ADDR            0xFCC00140
#define DEVICE_32K_B_CLOCK_ENABLE          0x2

#define SDIO_WAKEUP_GPIO_ADDR              0xFC80382C
#define SDIO_WAKEUP_GPIO_DIS_UP_PULLED     0xFFFFFFFE
#define SDIO_WAKEUP_GPIO_EN_DOWN_PULLED    0x2

#define ADDR_LEN                           32


#else 

#define DEVICE_32K_B_CLOCK_ADDR            0xFCC00140
#define DEVICE_32K_B_CLOCK_ENABLE          0x2

#define SDIO_WAKEUP_GPIO_ADDR              0xFC80382C
#define SDIO_WAKEUP_GPIO_DIS_UP_PULLED     0xFFFFFFFE
#define SDIO_WAKEUP_GPIO_EN_DOWN_PULLED    0x2

#define ADDR_LEN                           32

#endif

#define INIT_OK                    	1
#define DEV_SUC                    	0
#define DEV_FAIL                   	-1 
#define DEV_UART_CLK				(150000000) /* 150M */
#define DEV_DTS_32KB				"todev32kb"
#define DEV_DTS_UART				"uart3in1"
#define DEV_DTS_COMP_NAME 			"hisilicon,hi1101"
/* for huawei's BT, FM, GNSS, WLAN chip */

/*****************************************************************************
  3 STRUCT DEFINE
*****************************************************************************/

/*private data for pm driver*/
struct dev_board_info
{
/* hi110x gpio info */
    uint32 power_on;

    uint32 wlan_power_on;
    uint32 host_wakeup_wlan;
    uint32 wlan_wakeup_host;

    uint32 bfg_power_on;
    uint32 host_wakeup_bfg;
    uint32 bfg_wakeup_host;

/* 32K clock for hi1101 */
    const char * clock_name;
	struct clk* clk_32kb;
	struct clk* clk_uart;
	struct device_node *np;

/* hi110x gpio init flag 1:ok 0:no */
    uint32 init_flag;
};


/*****************************************************************************
  4 EXTERN VARIABLE
*****************************************************************************/
extern struct dev_board_info *g_wire_dev;

/*****************************************************************************
  5 EXTERN FUNCTION
*****************************************************************************/
extern int32 gpio_request_and_init(struct platform_device *pdev);
extern int32 hsad_hi110x_get_gpio(char *name);

#endif

