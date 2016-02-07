#ifndef _HUAWEI_GPS_H
#define _HUAWEI_GPS_H

#ifdef CONFIG_HWCONNECTIVITY
#include <linux/huawei/hw_connectivity.h>
#endif

#ifdef CONFIG_CONNECTIVITY_HI110X
extern int set_gps_ref_clk_enable_hi110x(bool enable);
#endif


#ifdef CONFIG_BCMGPS
extern int set_gps_ref_clk_enable_bcm(bool enable);
#endif

static inline int set_gps_ref_clk_enable(bool enable)
{
    int ret = 0;
#ifdef CONFIG_HWCONNECTIVITY
    if (isMyConnectivityChip(CHIP_TYPE_HI110X )) {
        #ifdef CONFIG_CONNECTIVITY_HI110X
        printk(KERN_INFO "cfg ps type is matched with hi110x, continue\n");
        ret = set_gps_ref_clk_enable_hi110x(enable);
        #endif
    } else {
        #ifdef CONFIG_BCMGPS
        ret = set_gps_ref_clk_enable_bcm(enable);
        #endif
    }
#else 

    #ifdef CONFIG_CONNECTIVITY_HI110X
        ret = set_gps_ref_clk_enable_hi110x(enable);
    #endif

    #ifdef CONFIG_BCMGPS
            ret = set_gps_ref_clk_enable_bcm(enable);
    #endif

#endif

    return ret;
}

#endif
