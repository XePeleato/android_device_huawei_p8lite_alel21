
#ifndef __HI6XXX_HKADC_VALUE_H__
#define __HI6XXX_HKADC_VALUE_H__
//#include "BSP.h"
//#include "mdrv_temp_cfg.h"
#include "soc_hkadc_interface.h"
#include <linux/hisi/hi6xxx_temp_cfg.h>

#if defined(BSP_CORE_MODEM) /*C核的系统适配*/
#include "stdio.h"

#define HKADC_DEBUG(p) printf p

/*CCPU实地址向虚拟地址映射的转换接口*/
#define THERMAL_CORE_SHARE_PHY2VIRT(phy)                (phy)

#elif defined (BSP_CORE_APP) /*A核的系统适配*/
#define HKADC_DEBUG(p) printk p

#elif defined (BSP_CORE_CM3)  /*M核的系统适配*/
#include "mcu_print.h"

#define HKADC_DEBUG(p) mcu_add_trace p

#define THERMAL_CORE_SHARE_PHY2VIRT(phy)         (phy - MCU_SYS_MEM_ADDR + M3_DDR_MEM_REMAP_START_ADDRESS)
#endif

/*type declare*/
#define INVALID_HKADC_LOGVALUE                0xff
#define INVALID_HKADC_PHYVALUE                0xFF

#define HKADC_VALUE_INVALID                   0xffff
#define HKADC_INVALID_CHAN_NUM                (0xffffffff)

/*无效温度值*/
#define TEMP_VALUE_INVALID                    (-255)

#define HKADC_INIT_MAGIC_CODE                 0x5A5A5A5A

#ifndef HKADC_OK
#define HKADC_OK               (0)
#endif

#ifndef HKADC_ERROR
#define HKADC_ERROR (-1)
#endif

#define HKADC_NULL      (void*)(0)
#define HKADC_FALSE            (0)
#define HKADC_TRUE             (1)


/*Register define*/
#define HKADC_REGBASE_ADDR        (SOC_HKADC_BASE_ADDR)  /*V8R1 HKADC为SOC IP*/

#define HKADC_START_ADDR          SOC_HKADC_DSP_START_ADDR(0)

#define HKADC_WR01_DATA_ADDR      SOC_HKADC_WR01_DATA_ADDR(0)
#define HKADC_WR23_DATA_ADDR      SOC_HKADC_WR23_DATA_ADDR(0)
#define HKADC_WR45_DATA_ADDR      SOC_HKADC_WR45_DATA_ADDR(0)
#define HKADC_WR67_DATA_ADDR      SOC_HKADC_WR67_DATA_ADDR(0)

#define HKADC_WR_NUM_ADDR         SOC_HKADC_WR_NUM_ADDR(0)

#define HKADC_DELAY01_ADDR        SOC_HKADC_DELAY01_ADDR(0)
#define HKADC_DELAY23_ADDR        SOC_HKADC_DELAY23_ADDR(0)
#define HKADC_DELAY45_ADDR        SOC_HKADC_DELAY45_ADDR(0)
#define HKADC_DELAY67_ADDR        SOC_HKADC_DELAY6_ADDR(0)

#define HKADC_RD0_DATA_ADDR       SOC_HKADC_DSP_RD0_DATA_ADDR(0)
#define HKADC_RD1_DATA_ADDR       SOC_HKADC_DSP_RD1_DATA_ADDR(0)
#define HKADC_RD2_DATA_ADDR       SOC_HKADC_DSP_RD2_DATA_ADDR(0)
#define HKADC_RD3_DATA_ADDR       SOC_HKADC_DSP_RD3_DATA_ADDR(0)
#define HKADC_RD4_DATA_ADDR       SOC_HKADC_DSP_RD4_DATA_ADDR(0)
#define HKADC_RD5_DATA_ADDR       SOC_HKADC_DSP_RD5_DATA_ADDR(0)
#define HKADC_RD6_DATA_ADDR       SOC_HKADC_DSP_RD6_DATA_ADDR(0)
#define HKADC_RD7_DATA_ADDR       SOC_HKADC_DSP_RD7_DATA_ADDR(0)

#define PMU_HKADC_CFG_ADDR          (0x00)
#define PMU_HKADC_START_ADDR        (0x01)
#define PMU_HKADC_STATUS_ADDR       (0x02)
#define PMU_HKADC_RDHIGH_ADDR       (0x03)
#define PMU_HKADC_RDLOW_ADDR        (0x04)

                              /*[(0写，1读)(1位) , (PMU_HKADC偏移地址)(7位) , (写的值，读无意义)(8位)]*/
#define HKADC_WR01_VALUE      ((0x0 << 31) | (PMU_HKADC_START_ADDR << 24) | (0x1 << 16) | (0x0 << 15) | (PMU_HKADC_CFG_ADDR << 8) | (0x3 << 5) )
#define HKADC_WR23_VALUE      ((0x1 << 31) | (PMU_HKADC_RDHIGH_ADDR << 24) | (0x1 << 15) | (PMU_HKADC_RDLOW_ADDR << 8))
#define HKADC_WR45_VALUE      ((0x0 << 15) | (PMU_HKADC_CFG_ADDR << 8) | (0x80 << 0))

#define HKADC_WR_NUM_VALUE    (5)

#define HKADC_DELAY01_VALUE   ((0x0700 << 16) | (0x0200 << 0))
#define HKADC_DELAY23_VALUE   ((0x00c8 << 16) | (0x00c8 << 0))

#define START_DELAY_TIMEOUT     (4000)

#define SLICE_ONE_MS  33

#define HKADC_REG(addr) (*(volatile unsigned int *)addr)

#define HKADC_BSP_REG_READ(addr, result) \
    ((result) = HKADC_REG(addr))

#define HKADC_BSP_REG_WRITE(addr, data) \
    (HKADC_REG(addr) = (data))

#define HKADC_REG_WRITE(reg,data) \
        HKADC_BSP_REG_WRITE(((char *)g_hkadc_baseadddr + reg), data)

#define HKADC_REG_READ(reg,result) \
        HKADC_BSP_REG_READ(((char *)g_hkadc_baseadddr + reg), result)

#define HKADC_VALUE_HIGH    (0x0FF0)
#define HKADC_VALUE_LOW     (0x000F)
#define HKADC_VALID_VALUE   (0x0FFF)

/*HKADC REF voltage*/
#define HKADC_VREF_1V8      1800
#define HKADC_VREF_25V      2500
#define HKADC_VREF_28V_VADD 2800

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
extern struct thermal_data_area *g_thermal_share_area;

extern struct drv_hkadc_map g_hkadc_chan_map[];

extern void __iomem *g_hkadc_baseadddr;

extern spinlock_t g_hkadc_lock;

extern int DRV_HKADC_GET_TEMP(HKADC_TEMP_PROTECT_E log_chan,
                unsigned short* volt,
                short* temp, HKADC_CONV_MODE_E ulMode);

extern struct drv_hkadc_map *bsp_hkadc_get_chan_map(int logic_chan);
extern int get_voltage_by_hkadc(unsigned char ucChan, unsigned short *voltage);
extern void hkadc_read_test(int channel);
extern void hkadc_tsensor_debug_show(void);//zhangran
extern int hkadc_fill_chan_map(unsigned short *p2l_tbl);

#endif   /*end of __HI6XXX_HKADC_VALUE_H__*/

