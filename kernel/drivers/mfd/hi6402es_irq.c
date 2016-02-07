/*
 * hisi_6402_irq.c  --  Interrupt controller support for Hisilicon HI6402ES
 *
 * Copyright (c) 2014 Hisilicon Technologies CO., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <linux/wakelock.h>
#include <linux/mutex.h>
#include <linux/clk.h>

#include <linux/mfd/hi6402es_irq.h>

#include <linux/irq.h>
#ifndef NO_IRQ
#define NO_IRQ 0
#endif

/* 8-bit register offset in irq */
#define HI6402ES_IRQ_CFG_BASE_ADDR	0x20007000

#define HI6402ES_REG_VERSION	(HI6402ES_IRQ_CFG_BASE_ADDR + 0x00)

#define HI6402ES_REG_IRQ_0	(HI6402ES_IRQ_CFG_BASE_ADDR + 0x14)
#define HI6402ES_REG_IRQ_1	(HI6402ES_IRQ_CFG_BASE_ADDR + 0x15)
#define HI6402ES_REG_IRQ_2	(HI6402ES_IRQ_CFG_BASE_ADDR + 0x16)
#define HI6402ES_REG_IRQM_0	(HI6402ES_IRQ_CFG_BASE_ADDR + 0x17)
#define HI6402ES_REG_IRQM_1	(HI6402ES_IRQ_CFG_BASE_ADDR + 0x18)
#define HI6402ES_REG_IRQM_2	(HI6402ES_IRQ_CFG_BASE_ADDR + 0x19)

#define HI6402ES_IRQ_REG_IOMUX	0x20001000

#define HI6402ES_IRQ_REG_IRQ_CTRL	0x20001218

/* read / write by ssi  */
#define	HI6402ES_CFG_SUB_START		0x20007000
#define	HI6402ES_CFG_SUB_END		0x20007FFF
#define	HI6402ES_PAGE_SELECT_MASK_SSI	0xFF
#define	HI6402ES_REG32_OFFSET_MASK_SSI	0xFC
#define	HI6402ES_REG_VAL_BIT		8
#define	HI6402ES_REG_VAL_MASK		0xFF
#define	HI6402ES_PAGE_SELECT_REG_0_SSI	0x1FD
#define	HI6402ES_PAGE_SELECT_REG_1_SSI	0x1FE
#define	HI6402ES_PAGE_SELECT_REG_2_SSI	0x1FF
#define	HI6402ES_RAM2AXI_RD_DATA0		(HI6402ES_IRQ_CFG_BASE_ADDR + 0x23)
#define	HI6402ES_RAM2AXI_RD_DATA1		(HI6402ES_IRQ_CFG_BASE_ADDR + 0x24)
#define	HI6402ES_RAM2AXI_RD_DATA2		(HI6402ES_IRQ_CFG_BASE_ADDR + 0x25)
#define	HI6402ES_RAM2AXI_RD_DATA3		(HI6402ES_IRQ_CFG_BASE_ADDR + 0x26)

#define HI6402ES_NR_IRQ		24
#define HI6402ES_MASK_FIELD	0xFF
#define HI6402ES_IRQ_BITS		8

/* vref & ibias power down reg */
#define HI6402ES_PLL_CLK_SEL_REG		(HI6402ES_IRQ_CFG_BASE_ADDR + 0x0A0)

#define HI6402ES_PLL_CLK_SEL_REG_BIT		0

#define HI6402ES_ANA_REG03		(HI6402ES_IRQ_CFG_BASE_ADDR + 0x0A4)

#define HI6402ES_VREF_FAST		HI6402ES_ANA_REG03
#define HI6402ES_VREF_FAST_BIT			6
#define HI6402ES_VREF_SEL			HI6402ES_ANA_REG03
#define HI6402ES_VREF_SEL_BIT			4
#define HI6402ES_IBIAS_PD			HI6402ES_ANA_REG03
#define HI6402ES_IBIAS_PD_BIT			0

#define HI6402ES_VREF_SEL_PD			0
#define HI6402ES_VREF_SEL_100K			1
#define HI6402ES_VREF_SEL_500K			2
#define HI6402ES_VREF_SEL_1K			3

#define HI6402ES_CP_CLK_DIV_REG		(HI6402ES_IRQ_CFG_BASE_ADDR + 0x0C6)

#define HI6402ES_POP_IRQ_CLK_PD_REG	(HI6402ES_IRQ_CFG_BASE_ADDR + 0x0C7)

#define HI6402ES_CP_CLK_DIV_REG_BIT		0

/* PLL */
#define HI6402ES_ANA_REG47		(HI6402ES_IRQ_CFG_BASE_ADDR + 0x0D0)

#define HI6402ES_PLL_SEL			HI6402ES_ANA_REG47
#define HI6402ES_PLL_SEL_BIT			7
#define HI6402ES_PLL_CAL_START		HI6402ES_ANA_REG47
#define HI6402ES_PLL_CAL_START_BIT		6
#define HI6402ES_PLL_RUN_START		HI6402ES_ANA_REG47
#define HI6402ES_PLL_RUN_START_BIT		5
#define HI6402ES_PLL_BYPASS		HI6402ES_ANA_REG47
#define HI6402ES_PLL_BYPASS_BIT			2
#define HI6402ES_PLL_PD_REG		HI6402ES_ANA_REG47
#define HI6402ES_PLL_PD_BIT			1

#define HI6402ES_ANA_REG48		(HI6402ES_IRQ_CFG_BASE_ADDR + 0x0D1)

#define HI6402ES_PLL_TRK_EN		HI6402ES_ANA_REG48
#define HI6402ES_PLL_TRK_EN_BIT			6

#define HI6402ES_ANA_REG50		(HI6402ES_IRQ_CFG_BASE_ADDR + 0x0D3)

#define HI6402ES_PLL_CLKTEST_EN	HI6402ES_ANA_REG50
#define HI6402ES_PLL_CLKTEST_EN_BIT	7
#define HI6402ES_PLL_PHE_THR_SEL	HI6402ES_ANA_REG50
#define HI6402ES_PLL_PHE_THR_SEL_BIT	2

#define HI6402ES_ANA_REG51		(HI6402ES_IRQ_CFG_BASE_ADDR + 0x0D4)
#define HI6402ES_PLL_DCO_INIT		HI6402ES_ANA_REG51
 
#define HI6402ES_ANA_REG54		(HI6402ES_IRQ_CFG_BASE_ADDR + 0x0D7)

#define HI6402ES_PLL_PDIV_AD		HI6402ES_ANA_REG54
#define HI6402ES_PLL_PDIVA		HI6402ES_ANA_REG54
#define HI6402ES_PLL_PDIVA_BIT			3
#define HI6402ES_PLL_PDIVD		HI6402ES_ANA_REG54
#define HI6402ES_PLL_PDIVD_BIT			1

#define HI6402ES_ANA_REG55		(HI6402ES_IRQ_CFG_BASE_ADDR + 0x0D8)

#define HI6402ES_PLL_LPF_PI		HI6402ES_ANA_REG55
#define HI6402ES_PLL_LPF_P		HI6402ES_ANA_REG55
#define HI6402ES_PLL_LPF_P_BIT			4
#define HI6402ES_PLL_LPF_I		HI6402ES_ANA_REG55
#define HI6402ES_PLL_LPF_I_BIT			0

#define HI6402ES_ANA_REG56		(HI6402ES_IRQ_CFG_BASE_ADDR + 0x0D9)
#define HI6402ES_PLL_PHE_SH		HI6402ES_ANA_REG56

#define HI6402ES_ANA_REG57		(HI6402ES_IRQ_CFG_BASE_ADDR + 0x0DA)

#define HI6402ES_PLL_FCW_1_0		HI6402ES_ANA_REG57
#define HI6402ES_PLL_FCW_1_0_BIT			6

#define HI6402ES_ANA_REG58		(HI6402ES_IRQ_CFG_BASE_ADDR + 0x0DB)
#define HI6402ES_PLL_FCW_9_2		HI6402ES_ANA_REG58

#define HI6402ES_ANA_REG59		(HI6402ES_IRQ_CFG_BASE_ADDR + 0x0DC)
#define HI6402ES_PLL_FCW_17_10		HI6402ES_ANA_REG59



#define HI6402ES_ANA_REG68		(HI6402ES_IRQ_CFG_BASE_ADDR + 0x0E5)

#define HI6402ES_ISO_A18_D12_REG		HI6402ES_ANA_REG68
#define HI6402ES_ISO_A18_D12_REG_BIT		1
#define HI6402ES_ISO_A33_D12_REG		HI6402ES_ANA_REG68
#define HI6402ES_ISO_A33_D12_REG_BIT		0

#define HI6402ES_ANA_REG69		(HI6402ES_IRQ_CFG_BASE_ADDR + 0x0E6)

#define HI6402ES_PLL_ISO_12_REG		HI6402ES_ANA_REG69
#define HI6402ES_PLL_ISO_12_REG_BIT		4
#define HI6402ES_PLL_RSTN_REFDIV		HI6402ES_ANA_REG69
#define HI6402ES_PLL_RSTN_REFDIV_BIT		3
#define HI6402ES_PLL_RSTN_REG		HI6402ES_ANA_REG69
#define HI6402ES_PLL_RSTN_REG_BIT			2
#define HI6402ES_PLL_PDIVD_EN		HI6402ES_ANA_REG69
#define HI6402ES_PLL_PDIVD_EN_BIT			1
#define HI6402ES_PLL_PDIVA_EN		HI6402ES_ANA_REG69
#define HI6402ES_PLL_PDIVA_EN_BIT			0

#define HI6402ES_ANA_REG75		(HI6402ES_IRQ_CFG_BASE_ADDR + 0x0F6)

#define HI6402ES_PLL_CALIB_FINISH		HI6402ES_ANA_REG75
#define HI6402ES_PLL_CALIB_FINISH_BIT		2
#define HI6402ES_PLL_LOCK			HI6402ES_ANA_REG75
#define HI6402ES_PLL_LOCK_BIT			1

#define HI6402ES_CODEC_ANA_ROREG5		(HI6402ES_IRQ_CFG_BASE_ADDR + 0x0EC)
#define HI6402ES_PLL_ISO_18_REG		HI6402ES_CODEC_ANA_ROREG5
#define HI6402ES_PLL_ISO_18_REG_BIT		4

#ifdef CONFIG_DEBUG_FS
#include <linux/debugfs.h>
#define	HI6402ES_DBG_PAGES		(9)
#define	HI6402ES_DBG_SIZE_PAGE		(0x1000)
static u32 hi6402es_page_reg0[] = {
	0x20007200+0x037,0x20007000+0x045,0x20007200+0x014,0x20007200+0x02E,
	0x20007200+0x034,0x20007200+0x02B,0x20007200+0x031,0x20007200+0x00C,
	0x20007200+0x00F,0x20007200+0x013,0x20007200+0x0AE,0x20007200+0x0AF,
	0x20007200+0x0C3,0x20007200+0x0C4,0x20007000+0x041,0x20007200+0x02A,
	0x20007200+0x02D,0x20007200+0x030,0x20007200+0x033,0x20007200+0x036,
	0x20007200+0x049,0x20007200+0x040,0x20007200+0x046,0x20007200+0x03D,
	0x20007200+0x043,0x20007200+0x0C5,0x20007200+0x0C6,0x20007200+0x0B0,
	0x20007200+0x0B1,0x20007000+0x042,0x20007200+0x03C,0x20007200+0x03F,
	0x20007200+0x042,0x20007200+0x045,0x20007200+0x048,0x20007200+0x058,
	0x20007200+0x04F,0x20007200+0x055,0x20007200+0x04C,0x20007200+0x052,
	0x20007200+0x00D,0x20007200+0x0C7,0x20007200+0x0C8,0x20007200+0x0B2,
	0x20007200+0x0B3,0x20007000+0x043,0x20007200+0x04B,0x20007200+0x04E,
	0x20007200+0x051,0x20007200+0x054,0x20007200+0x057,0x20007200+0x05E,
	0x20007200+0x064,0x20007200+0x05B,0x20007200+0x061,0x20007200+0x067,
	0x20007200+0x0C9,0x20007200+0x0CA,0x20007200+0x0B4,0x20007200+0x0B5,
	0x20007000+0x044,0x20007200+0x05A,0x20007200+0x05D,0x20007200+0x060,
	0x20007200+0x063,0x20007200+0x066,0x20007200+0x012,0x20007200+0x07D,
	0x20007200+0x07E,0x20007200+0x087,0x20007200+0x0B6,0x20007200+0x010,
	0x20007200+0x011,0x20007200+0x069,0x20007200+0x06A,0x20007200+0x06B,
	0x20007200+0x06C,0x20007200+0x06D,0x20007200+0x06E,0x20007000+0x047,
	0x20007200+0x00E,0x20007200+0x073,0x20007200+0x071,0x20007200+0x07A,
	0x20007200+0x078,0x20007200+0x015,0x20007200+0x086,0x20007000+0x046,
	0x20007200+0x1D0,0x20007200+0x0B7,0x20007200+0x0B8,0x20007200+0x0B9,
	0x20007200+0x0BA,0x20007200+0x0BB,0x20007200+0x0BC,0x20007000+0x049,
	0x20007200+0x089,0x20007200+0x08A,0x20007200+0x08B,0x20007200+0x080,
	0x20007200+0x083,0x20007200+0x081,0x20007200+0x084,0x20007200+0x090,
	0x20007200+0x091,0x20007200+0x08D,0x20007200+0x08C,0x20007200+0x008,
	0x20007200+0x009,0x20007200+0x00A,0x20007200+0x00B,0x20007200+0x092,
	0x20007200+0x0CB,0x20007200+0x0F1,0x20007200+0x094,0x20007200+0x093,
	0x20007200+0x095,0x20007000+0x04B,0x20007200+0x096,0x20007000+0x001,
	0x20007000+0x0A3,0x20007000+0x0BD,0x20007000+0x0C9,0x20007000+0x0A2,
	0x20007000+0x0B8,0x20007000+0x0B9,0x20007000+0x0B4,0x20007000+0x0B5,
	0x20007000+0x0A1,0x20007000+0x0AA,0x20007000+0x0A6,0x20007000+0x0CA,
	0x20007000+0x0A7,0x20007000+0x0AF,
};
static u32 hi6402es_page_reg1[] = {
	0x20007200+0x0B6,0x20007000+0x04A,0x20007200+0x097,0x20007000+0x04B,
	0x20007200+0x096,0x20007200+0x000,0x20007200+0x001,0x20007200+0x0C3,
	0x20007200+0x0C4,0x20007200+0x002,0x20007200+0x003,0x20007200+0x0C5,
	0x20007200+0x0C6,0x20007200+0x004,0x20007200+0x005,0x20007200+0x0C7,
	0x20007200+0x0C8,0x20007200+0x006,0x20007200+0x007,0x20007200+0x0C9,
	0x20007200+0x0CA,
};
static u32 hi6402es_page_reg2[] = {
	0x20007000+0x047,0x20007200+0x073,0x20007200+0x071,0x20007200+0x086,
	0x20007200+0x080,0x20007200+0x081,0x20007200+0x082,0x20007200+0x07A,
	0x20007200+0x078,0x20007000+0x04A,0x20007200+0x083,0x20007200+0x084,
	0x20007200+0x085,0x20007200+0x0B7,0x20007200+0x0B8,0x20007200+0x0B9,
	0x20007200+0x0BA,0x20007200+0x0BB,0x20007200+0x0BC,0x20007200+0x07D,
	0x20007200+0x07E,0x20007200+0x087,0x20007200+0x0B6,
};
static u32 hi6402es_page_reg3[] = {
	0x20007000+0x0A3,0x20007000+0x0BD,0x20007000+0x0C9,0x20007000+0x0A2,
	0x20007000+0x0B8,0x20007000+0x0B9,0x20007000+0x0B4,0x20007000+0x0B5,
	0x20007000+0x0A1,0x20007000+0x0AA,0x20007000+0x0A6,0x20007000+0x0CA,
	0x20007000+0x0A7,0x20007000+0x0AF,0x20007000+0x0C7,0x20007000+0x0B7,
	0x20007000+0x0C6,0x20007000+0x0EA,0x20007000+0x0EB,
};
static u32 hi6402es_page_reg4[] = {
	0x20007000+0x0DD,0x20007000+0x0E3,0x20007000+0x0E1,0x20007000+0x0E2,
	0x20007000+0x0E4,0x20007000+0x0E0,0x20007000+0x0D0,0x20007000+0x0D3,
	0x20007000+0x0F6,0x20007000+0x016,0x20007000+0x014,0x20007000+0x019,
	0x20007000+0x017,0x20007000+0x01A,0x20007000+0x01B,0x20007000+0x0DF,
	0x20007000+0x0DE,
};
static u32 hi6402es_page_reg5[] = {
	0x20007000+0x04A,0x20007200+0x0AA,0x20007000+0x004,0x20007200+0x1FC,
	0x20007200+0x0A9,0x20007200+0x0A2,0x20007200+0x0A3,0x20007200+0x0A4,
	0x20007200+0x1D4,0x20007200+0x1D6,0x20007200+0x1D5,0x20007200+0x09C,
	0x20007200+0x09D,0x20007200+0x0A1,0x20007200+0x0A5,0x20007200+0x0A6,
	0x20007200+0x0A7,0x20007200+0x0A8,0x20007200+0x0A0,0x20007200+0x098,
	0x20007200+0x099,0x20007200+0x09A,0x20007200+0x09B,0x20007200+0x09E,
	0x20007200+0x09F,0x20007000+0x013,
};
static u32 hi6402es_page_reg6[] = {
	0x20007000+0x053,0x20007000+0x054,0x20007000+0x057,0x20007000+0x05C,
	0x20007000+0x05D,0x20007000+0x001,0x20007000+0x05F,0x20007000+0x060,
	0x20007000+0x06C,0x20007000+0x06D,0x20007000+0x06E,0x20007000+0x070,
	0x20007000+0x071,0x20007000+0x072,0x20007000+0x073,0x20007000+0x074,
	0x20007000+0x075,0x20007000+0x076,0x20007000+0x077,0x20007000+0x078,
	0x20007000+0x079,
};
static u32 hi6402es_page_reg7[] = {
	0x20001000+0x000,0x20001000+0x100,0x20001000+0x104,0x20008000+0x400,
	0x20008000+0x404,0x20008000+0x408,0x20008000+0x40C,0x20008000+0x414,
	0x20008000+0x504,0x20008000+0x410,0x20008000+0x500,0x20008000+0x418,
	0x20008000+0x530,0x20008000+0x41C,0x20009000+0x400,0x20009000+0x404,
	0x20009000+0x408,0x20009000+0x40C,0x20009000+0x414,0x20009000+0x504,
	0x20009000+0x410,0x20009000+0x500,0x20009000+0x418,0x20009000+0x530,
	0x20009000+0x41C,0x2000A000+0x400,0x2000A000+0x404,0x2000A000+0x408,
	0x2000A000+0x40C,0x2000A000+0x414,0x2000A000+0x504,0x2000A000+0x410,
	0x2000A000+0x500,0x2000A000+0x418,0x2000A000+0x530,0x2000A000+0x41C,
	0x2000B000+0x400,0x2000B000+0x404,0x2000B000+0x408,0x2000B000+0x40C,
	0x2000B000+0x414,0x2000B000+0x504,0x2000B000+0x410,0x2000B000+0x500,
	0x2000B000+0x418,0x2000B000+0x530,0x2000B000+0x41C,
};
static u32 hi6402es_page_reg8[] = {
	0x20007000+0x054,0x20000000+0x0688,0x20003000+0x0100,0x20003000+0x0104,
	0x20000000+0x081C+0x40*0,0x20000000+0x0814+0x40*0,0x20000000+0x0818+0x40*0,0x20000000+0x0708+0x10*0,
	0x20000000+0x070c+0x10*0,0x20000000+0x0810+0x40*0,0x20003000+0x0110+0x4*0,0x20003000+0x0150+0x4*0,
	0x20000000+0x081C+0x40*1,0x20000000+0x0814+0x40*1,0x20000000+0x0818+0x40*1,0x20000000+0x0708+0x10*1,
	0x20000000+0x070c+0x10*1,0x20000000+0x0810+0x40*1,0x20003000+0x0110+0x4*1,0x20003000+0x0150+0x4*1,
	0x20000000+0x081C+0x40*2,0x20000000+0x0814+0x40*2,0x20000000+0x0818+0x40*2,0x20000000+0x0708+0x10*2,
	0x20000000+0x070c+0x10*2,0x20000000+0x0810+0x40*2,0x20003000+0x0110+0x4*2,0x20003000+0x0150+0x4*2,
	0x20000000+0x081C+0x40*3,0x20000000+0x0814+0x40*3,0x20000000+0x0818+0x40*3,0x20000000+0x0708+0x10*3,
	0x20000000+0x070c+0x10*3,0x20000000+0x0810+0x40*3,0x20003000+0x0110+0x4*3,0x20003000+0x0150+0x4*3,
	0x20000000+0x081C+0x40*4,0x20000000+0x0814+0x40*4,0x20000000+0x0818+0x40*4,0x20000000+0x0708+0x10*4,
	0x20000000+0x070c+0x10*4,0x20000000+0x0810+0x40*4,0x20003000+0x0110+0x4*4,0x20003000+0x0150+0x4*4,
	0x20000000+0x081C+0x40*5,0x20000000+0x0814+0x40*5,0x20000000+0x0818+0x40*5,0x20000000+0x0708+0x10*5,
	0x20000000+0x070c+0x10*5,0x20000000+0x0810+0x40*5,0x20003000+0x0110+0x4*5,0x20003000+0x0150+0x4*5,
	0x20000000+0x081C+0x40*6,0x20000000+0x0814+0x40*6,0x20000000+0x0818+0x40*6,0x20000000+0x0708+0x10*6,
	0x20000000+0x070c+0x10*6,0x20000000+0x0810+0x40*6,0x20003000+0x0110+0x4*6,0x20003000+0x0150+0x4*6,
	0x20000000+0x081C+0x40*7,0x20000000+0x0814+0x40*7,0x20000000+0x0818+0x40*7,0x20000000+0x0708+0x10*7,
	0x20000000+0x070c+0x10*7,0x20000000+0x0810+0x40*7,0x20003000+0x0110+0x4*7,0x20003000+0x0150+0x4*7,
	0x20000000+0x081C+0x40*8,0x20000000+0x0814+0x40*8,0x20000000+0x0818+0x40*8,0x20000000+0x0708+0x10*8,
	0x20000000+0x070c+0x10*8,0x20000000+0x0810+0x40*8,0x20003000+0x0110+0x4*8,0x20003000+0x0150+0x4*8,
	0x20000000+0x081C+0x40*9,0x20000000+0x0814+0x40*9,0x20000000+0x0818+0x40*9,0x20000000+0x0708+0x10*9,
	0x20000000+0x070c+0x10*9,0x20000000+0x0810+0x40*9,0x20003000+0x0110+0x4*9,0x20003000+0x0150+0x4*9,
	0x20000000+0x081C+0x40*10,0x20000000+0x0814+0x40*10,0x20000000+0x0818+0x40*10,0x20000000+0x0708+0x10*10,
	0x20000000+0x070c+0x10*10,0x20000000+0x0810+0x40*10,0x20003000+0x0110+0x4*10,0x20003000+0x0150+0x4*10,
	0x20000000+0x081C+0x40*11,0x20000000+0x0814+0x40*11,0x20000000+0x0818+0x40*11,0x20000000+0x0708+0x10*11,
	0x20000000+0x070c+0x10*11,0x20000000+0x0810+0x40*11,0x20003000+0x0110+0x4*11,0x20003000+0x0150+0x4*11,
	0x20000000+0x081C+0x40*12,0x20000000+0x0814+0x40*12,0x20000000+0x0818+0x40*12,0x20000000+0x0708+0x10*12,
	0x20000000+0x070c+0x10*12,0x20000000+0x0810+0x40*12,0x20003000+0x0110+0x4*12,0x20003000+0x0150+0x4*12,
	0x20000000+0x081C+0x40*13,0x20000000+0x0814+0x40*13,0x20000000+0x0818+0x40*13,0x20000000+0x0708+0x10*13,
	0x20000000+0x070c+0x10*13,0x20000000+0x0810+0x40*13,0x20003000+0x0110+0x4*13,0x20003000+0x0150+0x4*13,
	0x20000000+0x081C+0x40*14,0x20000000+0x0814+0x40*14,0x20000000+0x0818+0x40*14,0x20000000+0x0708+0x10*14,
	0x20000000+0x070c+0x10*14,0x20000000+0x0810+0x40*14,0x20003000+0x0110+0x4*14,0x20003000+0x0150+0x4*14,
	0x20000000+0x081C+0x40*15,0x20000000+0x0814+0x40*15,0x20000000+0x0818+0x40*15,0x20000000+0x0708+0x10*15,
	0x20000000+0x070c+0x10*15,0x20000000+0x0810+0x40*15,0x20003000+0x0110+0x4*15,0x20003000+0x0150+0x4*15,
};
static u32 hi6402es_page_reg9[] = {
	0x20007000+0x001,0x20007000+0x002,0x20007000+0x020,0x20007000+0x050,
	0x20007000+0x051,0x20007000+0x015,0x20007000+0x018,
};

/* instruction struct */
struct hi6402es_ins
{
	char ins;		/* w:wirte; r:read; p:read page. */
	u32	reg;
	u32	val;
};
struct hi6402es_ins g_hi6402es_ins;
spinlock_t	hi6402es_ins_lock;
/* g_data */
struct hi6402es_irq *hi6402es_gpirq;
static struct dentry *debug_dir;
#endif

static struct of_device_id of_hi6402es_irq_child_match_tbl[] = {
	/* codec */
	{
		.compatible = "hisilicon,hi6402es-codec",
	},
	/* mbhc */
	{
		.compatible = "hisilicon,hi6402es-mbhc",
	},
	/* hifi_misc */
	{
		.compatible = "hisilicon,hi6402es-dsp",
	},
	{ /* end */ }
};

static struct of_device_id of_hi6402es_irq_match[] = {
	{
		.compatible = "hisilicon,hi6402es-irq",
	},
	{ /* end */ }
};

static void hi6402es_select_reg_page_ssi(struct hi6402es_irq *irq, unsigned int reg )
{
	unsigned int reg_page = reg & (~HI6402ES_PAGE_SELECT_MASK_SSI);
	if(irq->page_sel_ssi == reg_page) {
		pr_debug("%s: page_sel is the same.\n",__FUNCTION__);
		return;
	} else {
		pr_debug("%s: page_sel is the different.\n",__FUNCTION__);
		irq->page_sel_ssi = reg_page;
	}
	reg_page = reg_page >> HI6402ES_REG_VAL_BIT;

	writel(reg_page & HI6402ES_REG_VAL_MASK,
			irq->reg_base_addr + (HI6402ES_PAGE_SELECT_REG_0_SSI << 2));

	reg_page = reg_page >> HI6402ES_REG_VAL_BIT;
	writel(reg_page & HI6402ES_REG_VAL_MASK,
			irq->reg_base_addr + (HI6402ES_PAGE_SELECT_REG_1_SSI << 2));

	reg_page = reg_page >> HI6402ES_REG_VAL_BIT;
	writel(reg_page & HI6402ES_REG_VAL_MASK,
			irq->reg_base_addr + (HI6402ES_PAGE_SELECT_REG_2_SSI << 2));
}

static unsigned int hi6402es_reg_read_8bit_ssi(struct hi6402es_irq *irq, unsigned int reg)
{
	hi6402es_select_reg_page_ssi( irq, reg );
	return readl((void*)irq->reg_base_addr
			+ ((reg & (HI6402ES_REG_VAL_MASK)) << 2));
}
static unsigned int hi6402es_reg_read_32bit_ssi(struct hi6402es_irq *irq, unsigned int reg)
{
	u32 ret = 0;

	hi6402es_select_reg_page_ssi( irq, reg );
	ret = readl((void*)irq->reg_base_addr + ((reg & (HI6402ES_REG32_OFFSET_MASK_SSI)) << 2));

	hi6402es_select_reg_page_ssi( irq, HI6402ES_RAM2AXI_RD_DATA0 );
	ret = readl((void*)irq->reg_base_addr +
			(((HI6402ES_RAM2AXI_RD_DATA3 ) & (HI6402ES_REG_VAL_MASK)) << 2));
	ret = (ret<<8) + (0xFF & readl((void*)irq->reg_base_addr +
				(((HI6402ES_RAM2AXI_RD_DATA2) & (HI6402ES_REG_VAL_MASK)) << 2)));
	ret = (ret<<8) + (0xFF & readl((void*)irq->reg_base_addr +
				(((HI6402ES_RAM2AXI_RD_DATA1) & (HI6402ES_REG_VAL_MASK)) << 2)));
	ret = (ret<<8) + (0xFF & readl((void*)irq->reg_base_addr +
				(((HI6402ES_RAM2AXI_RD_DATA0) & (HI6402ES_REG_VAL_MASK)) << 2)));

	return ret ;
}
static unsigned int hi6402es_reg_read_ssi(struct hi6402es_irq *irq, unsigned int reg)
{
	unsigned int ret = 0;

	if (reg < HI6402ES_CFG_SUB_START || reg > HI6402ES_CFG_SUB_END)
		ret = hi6402es_reg_read_32bit_ssi(irq, reg);
	else
		ret = hi6402es_reg_read_8bit_ssi(irq, reg);

	return ret;
}
static void hi6402es_reg_write_8bit_ssi(struct hi6402es_irq *irq, unsigned int reg, unsigned int val)
{
	hi6402es_select_reg_page_ssi(irq, reg);
	writel(val & HI6402ES_REG_VAL_MASK ,
			(void*)irq->reg_base_addr + ((reg & HI6402ES_REG_VAL_MASK) << 2));
}

void hi6402es_reg_write_32bit_ssi(struct hi6402es_irq *irq, unsigned int reg, unsigned int val)
{
	if (reg & 0x3) {
		pr_err("%s:reg is 0x%x, it's not alignment!!\n",__FUNCTION__,reg);
		return;
	}

	hi6402es_select_reg_page_ssi( irq, reg );
	writel(val & HI6402ES_REG_VAL_MASK ,
			(void*)irq->reg_base_addr + ((reg & HI6402ES_REG_VAL_MASK) << 2));
	writel((val>>8) & HI6402ES_REG_VAL_MASK ,
			(void*)irq->reg_base_addr + (((reg+1) & HI6402ES_REG_VAL_MASK) << 2));
	writel((val>>16) & HI6402ES_REG_VAL_MASK ,
			(void*)irq->reg_base_addr + (((reg+2) & HI6402ES_REG_VAL_MASK) << 2));
	writel((val>>24) & HI6402ES_REG_VAL_MASK ,
			(void*)irq->reg_base_addr + (((reg+3) & HI6402ES_REG_VAL_MASK) << 2));
}

static void hi6402es_reg_write_ssi(struct hi6402es_irq *irq, unsigned int reg, unsigned int val)
{
	if(reg < HI6402ES_CFG_SUB_START || reg > HI6402ES_CFG_SUB_END)
		hi6402es_reg_write_32bit_ssi( irq, reg, val);
	else
		hi6402es_reg_write_8bit_ssi( irq, reg, val);
}

/*
 * The irq register is only 8-bit.
 * Hisilicon SoC use hardware to map irq register into SoC mapping.
 * At here, we are accessing SoC register with 32-bit.
 */
u32 hi6402es_irq_read(struct hi6402es_irq *irq, unsigned int reg)
{
	u32 ret = 0;

	mutex_lock(&irq->mutex);
	ret = hi6402es_reg_read_ssi(irq, reg);
	mutex_unlock(&irq->mutex);

	return ret;
}
EXPORT_SYMBOL(hi6402es_irq_read);

void hi6402es_irq_write(struct hi6402es_irq *irq, unsigned int reg, u32 val)
{
	mutex_lock(&irq->mutex);
	hi6402es_reg_write_ssi(irq, reg, val);
	mutex_unlock(&irq->mutex);
}
EXPORT_SYMBOL(hi6402es_irq_write);

void hi6402es_reg_set_bit(struct hi6402es_irq *irq,
		unsigned int reg, unsigned int offset)
{
	unsigned int val = 0;

	BUG_ON(NULL == irq);

	mutex_lock(&irq->mutex);
	val = hi6402es_reg_read_ssi(irq, reg) | (1 << offset);
	hi6402es_reg_write_ssi(irq, reg, val);
	mutex_unlock(&irq->mutex);
}
EXPORT_SYMBOL(hi6402es_reg_set_bit);

void hi6402es_reg_clr_bit(struct hi6402es_irq *irq,
		unsigned int reg, unsigned int offset)
{
	unsigned int val = 0;

	BUG_ON(NULL == irq);

	mutex_lock(&irq->mutex);
	val = hi6402es_reg_read_ssi(irq, reg) & ~(1 << offset);
	hi6402es_reg_write_ssi(irq, reg, val);
	mutex_unlock(&irq->mutex);
}
EXPORT_SYMBOL(hi6402es_reg_clr_bit);

void hi6402es_reg_write_bits(struct hi6402es_irq *irq,
		unsigned int reg, unsigned int value, unsigned int mask)
{
	unsigned int val = 0;

	BUG_ON(NULL == irq);

	mutex_lock(&irq->mutex);
	val = hi6402es_reg_read_ssi(irq, reg);
	val &= ~mask;
	val |= value & mask;
	hi6402es_reg_write_ssi(irq, reg, val);
	mutex_unlock(&irq->mutex);
}
EXPORT_SYMBOL(hi6402es_reg_write_bits);

static void hi6402es_irq_enable_ibias(struct hi6402es_irq *irq, bool enable)
{
#if 0
	int ret = 0;
#endif
	BUG_ON(NULL == irq);

	if (enable) {
#if 0
		/* enable ldo8 */
		ret = regulator_bulk_enable(1, &irq->regu);
		if (0 != ret) {
			pr_err("%s : couldn't enable regulators %d\n",
					__FUNCTION__, ret);
			return;
		}
#endif
		/* vref_ibias_on */
		hi6402es_reg_write_bits(irq, HI6402ES_VREF_SEL,
				HI6402ES_VREF_SEL_1K<<HI6402ES_VREF_SEL_BIT, 0x03<<HI6402ES_VREF_SEL_BIT);
		hi6402es_reg_set_bit(irq, HI6402ES_VREF_FAST, HI6402ES_VREF_FAST_BIT);
		msleep(30);
		hi6402es_reg_clr_bit(irq, HI6402ES_VREF_FAST, HI6402ES_VREF_FAST_BIT);
		hi6402es_reg_write_bits(irq, HI6402ES_VREF_SEL,
				HI6402ES_VREF_SEL_100K<<HI6402ES_VREF_SEL_BIT, 0x03<<HI6402ES_VREF_SEL_BIT);
		hi6402es_reg_clr_bit(irq, HI6402ES_IBIAS_PD, HI6402ES_IBIAS_PD_BIT);
	} else {
		hi6402es_reg_set_bit(irq, HI6402ES_IBIAS_PD, HI6402ES_IBIAS_PD_BIT);
		hi6402es_reg_write_bits(irq, HI6402ES_VREF_SEL,
				HI6402ES_VREF_SEL_PD<<HI6402ES_VREF_SEL_BIT, 0x03<<HI6402ES_VREF_SEL_BIT);
#if 0
		regulator_bulk_disable(1, &irq->regu);
#endif
	}
}

void hi6402es_irq_ibias_work_enable(struct hi6402es_irq *irq, bool enable)
{
	BUG_ON(NULL == irq);

	mutex_lock(&irq->ibias_mutex);
	if (enable) {
		if (0 == irq->ibias_work)
			hi6402es_irq_enable_ibias(irq, true);
		++irq->ibias_work;
	} else {
		--irq->ibias_work;
		if (0 == irq->ibias_work)
			hi6402es_irq_enable_ibias(irq, false);
	}
	mutex_unlock(&irq->ibias_mutex);
}
EXPORT_SYMBOL(hi6402es_irq_ibias_work_enable);

static inline void hi6402es_irq_pll_mode_cfg(struct hi6402es_irq *irq, enum hi6402es_pll_status status)
{
	switch(status) {
	case HI6402ES_PLL_LOW_FREQ:
		hi6402es_reg_set_bit(irq, HI6402ES_PLL_SEL, HI6402ES_PLL_SEL_BIT);
		hi6402es_irq_write(irq, HI6402ES_PLL_FCW_17_10, 0x05);
		hi6402es_irq_write(irq, HI6402ES_PLL_FCW_9_2, 0xdc);
		hi6402es_reg_write_bits(irq, HI6402ES_PLL_FCW_1_0,
				0x0<<HI6402ES_PLL_FCW_1_0_BIT, 0x3<<HI6402ES_PLL_FCW_1_0_BIT);
		hi6402es_irq_write(irq, HI6402ES_PLL_PDIV_AD, 0x87);
		hi6402es_irq_write(irq, HI6402ES_PLL_LPF_PI, 0x29);
		hi6402es_irq_write(irq, HI6402ES_PLL_DCO_INIT, 0x80);
		hi6402es_irq_write(irq, HI6402ES_PLL_PHE_SH, 0x01);
		hi6402es_reg_clr_bit(irq, HI6402ES_PLL_CLKTEST_EN, HI6402ES_PLL_CLKTEST_EN_BIT);
		hi6402es_reg_set_bit(irq, HI6402ES_PLL_PHE_THR_SEL, HI6402ES_PLL_PHE_THR_SEL_BIT);
		break;
	case HI6402ES_PLL_HIGH_FREQ:
		hi6402es_reg_clr_bit(irq, HI6402ES_PLL_SEL, HI6402ES_PLL_SEL_BIT);
		hi6402es_irq_write(irq, HI6402ES_PLL_FCW_17_10, 0x70);
		hi6402es_irq_write(irq, HI6402ES_PLL_FCW_9_2, 0xA3);
		hi6402es_reg_write_bits(irq, HI6402ES_PLL_FCW_1_0,
				0x3<<HI6402ES_PLL_FCW_1_0_BIT, 0x3<<HI6402ES_PLL_FCW_1_0_BIT);
		hi6402es_irq_write(irq, HI6402ES_PLL_PDIV_AD, 0xDB);
		hi6402es_irq_write(irq, HI6402ES_PLL_LPF_PI, 0x69);
		hi6402es_irq_write(irq, HI6402ES_PLL_DCO_INIT, 0x80);
		hi6402es_irq_write(irq, HI6402ES_PLL_PHE_SH, 0x00);
		hi6402es_reg_clr_bit(irq, HI6402ES_PLL_CLKTEST_EN, HI6402ES_PLL_CLKTEST_EN_BIT);
		hi6402es_reg_clr_bit(irq, HI6402ES_PLL_PHE_THR_SEL, HI6402ES_PLL_PHE_THR_SEL_BIT);
		break;
	default:
		pr_err("no cmd %d.\n", status);
		break;
	}
	return;
}

void hi6402es_irq_mad_enable_work(struct hi6402es_irq *irq, bool enable)
{
	int ret = 0;

	BUG_ON(NULL == irq);

	if (enable) {
		hi6402es_irq_ibias_work_enable(irq, true);
		ret = clk_prepare_enable(irq->pmu_audio_clk);
		if (0 != ret) {
			pr_err("pmu_audio_clk :clk prepare enable failed !\n");
			return;
		}
		/* shutdown cp clk before clk switch */
		hi6402es_reg_set_bit(irq, HI6402ES_POP_IRQ_CLK_PD_REG,0 );
		/* 20007004  1b 1   */
		/* 20007004  3b 0   */
		/* 19.2M -> pll clk */
		hi6402es_reg_set_bit(irq, HI6402ES_PLL_CLK_SEL_REG, HI6402ES_PLL_CLK_SEL_REG_BIT);
		/* CP div 256 */
		hi6402es_reg_write_bits(irq, HI6402ES_CP_CLK_DIV_REG, 0x07, 0x07);
		hi6402es_reg_clr_bit(irq, HI6402ES_POP_IRQ_CLK_PD_REG,0 );
		/* PLL cfg */
		hi6402es_irq_pll_mode_cfg(irq, HI6402ES_PLL_LOW_FREQ);
		hi6402es_irq_write(irq, HI6402ES_ANA_REG69, 0x00);
		hi6402es_irq_write(irq, HI6402ES_PLL_TRK_EN, 0x14);
		udelay(10);
		hi6402es_reg_set_bit(irq, HI6402ES_PLL_RSTN_REG, HI6402ES_PLL_RSTN_REG_BIT);
		hi6402es_reg_set_bit(irq, HI6402ES_PLL_RUN_START, HI6402ES_PLL_RUN_START_BIT);
		hi6402es_reg_set_bit(irq, HI6402ES_PLL_CAL_START, HI6402ES_PLL_CAL_START_BIT);
		hi6402es_reg_clr_bit(irq, HI6402ES_PLL_PD_REG, HI6402ES_PLL_PD_BIT);
		hi6402es_reg_set_bit(irq, HI6402ES_PLL_RSTN_REFDIV, HI6402ES_PLL_RSTN_REFDIV_BIT);
		hi6402es_reg_set_bit(irq, HI6402ES_PLL_ISO_12_REG, HI6402ES_PLL_ISO_12_REG_BIT);
		hi6402es_reg_set_bit(irq, HI6402ES_PLL_ISO_18_REG, HI6402ES_PLL_ISO_18_REG_BIT);
		udelay(200);
		hi6402es_reg_set_bit(irq, HI6402ES_PLL_PDIVD_EN, HI6402ES_PLL_PDIVD_EN_BIT);
		hi6402es_reg_set_bit(irq, HI6402ES_PLL_PDIVA_EN, HI6402ES_PLL_PDIVA_EN_BIT);
		/* shutdown cp clk before clk switch*/
		hi6402es_reg_set_bit(irq, HI6402ES_POP_IRQ_CLK_PD_REG,0 );
		/* PLL not BYPASS */
		hi6402es_reg_clr_bit(irq, HI6402ES_PLL_BYPASS, HI6402ES_PLL_BYPASS_BIT);
		hi6402es_reg_clr_bit(irq, HI6402ES_POP_IRQ_CLK_PD_REG,0 );
		irq->pll_status = HI6402ES_PLL_LOW_FREQ;
	} else {
		/* shutdown cp clk before clk switch*/
		hi6402es_reg_set_bit(irq, HI6402ES_POP_IRQ_CLK_PD_REG,0 );
		/* PLL BYPASS */
		hi6402es_reg_set_bit(irq, HI6402ES_PLL_BYPASS, HI6402ES_PLL_BYPASS_BIT);
		/* 32k -> pll clk */
		hi6402es_reg_set_bit(irq, HI6402ES_PLL_CLK_SEL_REG, HI6402ES_PLL_CLK_SEL_REG_BIT);
		/* CP div 2 */
		hi6402es_reg_write_bits(irq, HI6402ES_CP_CLK_DIV_REG, 0x00, 0x07);
		hi6402es_reg_clr_bit(irq, HI6402ES_POP_IRQ_CLK_PD_REG,0 );

		hi6402es_reg_clr_bit(irq, HI6402ES_PLL_PDIVA_EN, HI6402ES_PLL_PDIVA_EN_BIT);
		hi6402es_reg_clr_bit(irq, HI6402ES_PLL_PDIVD_EN, HI6402ES_PLL_PDIVD_EN_BIT);
		hi6402es_reg_set_bit(irq, HI6402ES_PLL_PD_REG, HI6402ES_PLL_PD_BIT);
		hi6402es_reg_clr_bit(irq, HI6402ES_PLL_RSTN_REG, HI6402ES_PLL_RSTN_REG_BIT);
		hi6402es_reg_clr_bit(irq, HI6402ES_PLL_RSTN_REFDIV, HI6402ES_PLL_RSTN_REFDIV_BIT);

		clk_disable_unprepare(irq->pmu_audio_clk);
		hi6402es_irq_ibias_work_enable(irq, false);
		irq->pll_status = HI6402ES_PLL_PD;
	}

	return;
}

void hi6402es_irq_pll_enable_work(struct hi6402es_irq *irq, bool enable)
{
	int ret = 0;

	BUG_ON(NULL == irq);

	if (enable) {
		hi6402es_irq_ibias_work_enable(irq, true);
		ret = clk_prepare_enable(irq->pmu_audio_clk);
		if (0 != ret) {
			pr_err("pmu_audio_clk :clk prepare enable failed !\n");
			return;
		}
		/* shutdown cp clk before clk switch */
		hi6402es_reg_set_bit(irq, HI6402ES_POP_IRQ_CLK_PD_REG,0 );
		/* 19.2M -> pll clk */
		hi6402es_reg_clr_bit(irq, HI6402ES_PLL_CLK_SEL_REG, HI6402ES_PLL_CLK_SEL_REG_BIT);
		/* CP div 256 */
		hi6402es_reg_write_bits(irq, HI6402ES_CP_CLK_DIV_REG, 0x07, 0x07);
		hi6402es_reg_clr_bit(irq, HI6402ES_POP_IRQ_CLK_PD_REG,0 );
		/* PLL cfg */
		hi6402es_irq_pll_mode_cfg(irq, HI6402ES_PLL_HIGH_FREQ);
		hi6402es_irq_write(irq, HI6402ES_ANA_REG69, 0x00);
		hi6402es_irq_write(irq, HI6402ES_PLL_TRK_EN, 0x14);
		udelay(10);
		hi6402es_reg_set_bit(irq, HI6402ES_PLL_RSTN_REG, HI6402ES_PLL_RSTN_REG_BIT);
		hi6402es_reg_set_bit(irq, HI6402ES_PLL_RUN_START, HI6402ES_PLL_RUN_START_BIT);
		hi6402es_reg_set_bit(irq, HI6402ES_PLL_CAL_START, HI6402ES_PLL_CAL_START_BIT);
		hi6402es_reg_clr_bit(irq, HI6402ES_PLL_PD_REG, HI6402ES_PLL_PD_BIT);
		hi6402es_reg_set_bit(irq, HI6402ES_PLL_RSTN_REFDIV, HI6402ES_PLL_RSTN_REFDIV_BIT);
		hi6402es_reg_set_bit(irq, HI6402ES_PLL_ISO_12_REG, HI6402ES_PLL_ISO_12_REG_BIT);
		hi6402es_reg_set_bit(irq, HI6402ES_PLL_ISO_18_REG, HI6402ES_PLL_ISO_18_REG_BIT);
		udelay(200);
		hi6402es_reg_set_bit(irq, HI6402ES_PLL_PDIVD_EN, HI6402ES_PLL_PDIVD_EN_BIT);
		hi6402es_reg_set_bit(irq, HI6402ES_PLL_PDIVA_EN, HI6402ES_PLL_PDIVA_EN_BIT);
		/* shutdown cp clk before clk switch*/
		hi6402es_reg_set_bit(irq, HI6402ES_POP_IRQ_CLK_PD_REG,0 );
		/* PLL not BYPASS */
		hi6402es_reg_clr_bit(irq, HI6402ES_PLL_BYPASS, HI6402ES_PLL_BYPASS_BIT);
		hi6402es_reg_clr_bit(irq, HI6402ES_POP_IRQ_CLK_PD_REG,0 );
		irq->pll_status = HI6402ES_PLL_HIGH_FREQ;
	} else {
		/* shutdown cp clk before clk switch*/
		hi6402es_reg_set_bit(irq, HI6402ES_POP_IRQ_CLK_PD_REG,0 );
		/* PLL BYPASS */
		hi6402es_reg_set_bit(irq, HI6402ES_PLL_BYPASS, HI6402ES_PLL_BYPASS_BIT);
		/* 32k -> pll clk */
		hi6402es_reg_set_bit(irq, HI6402ES_PLL_CLK_SEL_REG, HI6402ES_PLL_CLK_SEL_REG_BIT);
		/* CP div 2 */
		hi6402es_reg_write_bits(irq, HI6402ES_CP_CLK_DIV_REG, 0x00, 0x07);
		hi6402es_reg_clr_bit(irq, HI6402ES_POP_IRQ_CLK_PD_REG,0 );

		hi6402es_reg_clr_bit(irq, HI6402ES_PLL_PDIVA_EN, HI6402ES_PLL_PDIVA_EN_BIT);
		hi6402es_reg_clr_bit(irq, HI6402ES_PLL_PDIVD_EN, HI6402ES_PLL_PDIVD_EN_BIT);
		hi6402es_reg_set_bit(irq, HI6402ES_PLL_PD_REG, HI6402ES_PLL_PD_BIT);
		hi6402es_reg_clr_bit(irq, HI6402ES_PLL_RSTN_REG, HI6402ES_PLL_RSTN_REG_BIT);
		hi6402es_reg_clr_bit(irq, HI6402ES_PLL_RSTN_REFDIV, HI6402ES_PLL_RSTN_REFDIV_BIT);
		
		clk_disable_unprepare(irq->pmu_audio_clk);

		hi6402es_irq_ibias_work_enable(irq, false);
		irq->pll_status = HI6402ES_PLL_PD;
	}
}

static void hi6402es_irq_set_pll_mode(struct hi6402es_irq *irq)
{
	BUG_ON(NULL == irq);

	switch(irq->pll_status) {
		case HI6402ES_PLL_PD:
			if (irq->ref_pll)
				hi6402es_irq_pll_enable_work(irq, true);
			else if (irq->mad_need_pll)
				hi6402es_irq_mad_enable_work(irq, true);
			else
				/* todo */;
			break;
		case HI6402ES_PLL_HIGH_FREQ:
			if (0 == irq->ref_pll) {
				hi6402es_irq_pll_enable_work(irq, false);
				if(irq->mad_need_pll)
					hi6402es_irq_mad_enable_work(irq, true);
			}
			break;
		case HI6402ES_PLL_LOW_FREQ:
			if (0 == irq->mad_need_pll || irq->ref_pll)
				hi6402es_irq_mad_enable_work(irq, false);
			if (irq->ref_pll)
				hi6402es_irq_pll_enable_work(irq, true);
			break;
		default:
			break;
	}
	return;
}

void hi6402es_irq_mad_pll_enable(struct hi6402es_irq *irq, bool enable)
{
	BUG_ON(NULL == irq);

	mutex_lock(&irq->pll_mutex);
	if (enable) {
		irq->mad_need_pll++;
		if (1 == irq->mad_need_pll)
			hi6402es_irq_set_pll_mode(irq);
	} else {
		irq->mad_need_pll--;
		if (0 == irq->mad_need_pll)
			hi6402es_irq_set_pll_mode(irq);
	}
	mutex_unlock(&irq->pll_mutex);

	return;
}
EXPORT_SYMBOL(hi6402es_irq_mad_pll_enable);

void hi6402es_irq_pll_enable(struct hi6402es_irq *irq, bool enable)
{
	BUG_ON(NULL == irq);
	mutex_lock(&irq->pll_mutex);
	if(enable) {
		irq->ref_pll++;
		if(1 == irq->ref_pll)
			hi6402es_irq_set_pll_mode(irq);
	} else {
		irq->ref_pll--;
		if(0 == irq->ref_pll)
			hi6402es_irq_set_pll_mode(irq);
	}
	mutex_unlock(&irq->pll_mutex);
	return;
}
EXPORT_SYMBOL(hi6402es_irq_pll_enable);

void hi6402es_irq_work_func(struct work_struct *work)
{
	struct hi6402es_irq *hi6402es_irq = 
		container_of(work, struct hi6402es_irq, hi6402es_irq_delay_work.work);
	unsigned long pending = 0;
	int offset = 0;
	int irqs_offset = 0;
	int i = 0;
	unsigned long flags = 0;

	BUG_ON(NULL == hi6402es_irq);

	mutex_lock(&hi6402es_irq->sr_mutex);

	for (i = HI6402ES_REG_IRQ_0; i <= HI6402ES_REG_IRQ_2; i++) {
		/* 8 irqs in each irq reg */
		irqs_offset = 8 * (i - HI6402ES_REG_IRQ_0);
		pending = hi6402es_irq_read(hi6402es_irq, i);
		pending &= HI6402ES_MASK_FIELD;

		if (0 == pending)
			continue;

		/* clr all irqs */
		hi6402es_irq_write(hi6402es_irq, i, pending);
		/* IRQMx reg addr is IRQX + 3 */
		pending &= (~hi6402es_irq_read(hi6402es_irq, i + 3));
	
		spin_lock_irqsave(&hi6402es_irq->lock, flags);
		/* handle each irq */
		if (pending)
			for_each_set_bit(offset, &pending, HI6402ES_IRQ_BITS)
					generic_handle_irq(hi6402es_irq->irqs[offset + irqs_offset]);
		spin_unlock_irqrestore(&hi6402es_irq->lock, flags);
	}

	enable_irq(hi6402es_irq->irq);

	wake_lock_timeout(&hi6402es_irq->wake_lock, 3000);
	mutex_unlock(&hi6402es_irq->sr_mutex);
}

static irqreturn_t hi6402es_irq_handler(int irq, void *data)
{
	struct hi6402es_irq *hi6402es_irq = (struct hi6402es_irq *)data;

	if (!hi6402es_irq) {
		pr_err("hi6402es_irq is null\n");
		return IRQ_HANDLED;
	}

	disable_irq_nosync(hi6402es_irq->irq);
	queue_delayed_work(hi6402es_irq->hi6402es_irq_delay_wq,
			&hi6402es_irq->hi6402es_irq_delay_work,
			msecs_to_jiffies(0));

	return IRQ_HANDLED;
}

static void hi6402es_irq_mask(struct irq_data *d)
{
	struct hi6402es_irq *hi6402es_irq = irq_data_get_irq_chip_data(d);
	unsigned int id = irqd_to_hwirq(d);
	unsigned int mask_offset = 0;

	/* get reg addr offset */
	mask_offset = HI6402ES_REG_IRQM_0 + (id >> 3);

	hi6402es_reg_set_bit(hi6402es_irq, mask_offset, (id & 0x07));
}

static void hi6402es_irq_unmask(struct irq_data *d)
{
	struct hi6402es_irq *hi6402es_irq = irq_data_get_irq_chip_data(d);
	unsigned int id = irqd_to_hwirq(d);
	unsigned int irq_offset = 0;
	unsigned int mask_offset = 0;

	/* get reg addr offset */
	irq_offset = HI6402ES_REG_IRQ_0 + (id >> 3);
	mask_offset = HI6402ES_REG_IRQM_0 + (id >> 3);

	/* clear irq */
	hi6402es_irq_write(hi6402es_irq, irq_offset, 1 << (id & 0x07));

	hi6402es_reg_clr_bit(hi6402es_irq, mask_offset, (id & 0x07));
}

static struct irq_chip hi6402es_irq_chip = {
	.name		= "hi6402es_irq",
	.irq_mask	= hi6402es_irq_mask,
	.irq_unmask	= hi6402es_irq_unmask,
};

static int hi6402es_irq_map(struct irq_domain *d, unsigned int virq,
			irq_hw_number_t hw)
{
	struct hi6402es_irq *irq = d->host_data;

	irq_set_chip_and_handler_name(virq, &hi6402es_irq_chip,
				      handle_simple_irq, "hi6402es_irq");
	irq_set_chip_data(virq, irq);
	irq_set_irq_type(virq, IRQ_TYPE_NONE);

	return 0;
}

static struct irq_domain_ops hi6402es_domain_ops = {
	.map	= hi6402es_irq_map,
	.xlate	= irq_domain_xlate_twocell,
};

static int codec_ssi_iomux_default(struct pinctrl *pctrl)
{
	struct pinctrl_state *pinctrl_state;

	BUG_ON(NULL == pctrl);

	pinctrl_state = pinctrl_lookup_state(pctrl, PINCTRL_STATE_DEFAULT);

	if (IS_ERR(pinctrl_state)) {
		pr_err("%s : could not get default state (%li)\n",
				__FUNCTION__ , PTR_ERR(pinctrl_state));
		return -1;
	}

	if (pinctrl_select_state(pctrl, pinctrl_state)) {
		pr_err("%s : could not set pins to default state\n", __FUNCTION__);
		return -1;
	}

	return 0;
}

static int codec_ssi_iomux_idle(struct pinctrl *pctrl)
{
	struct pinctrl_state *pinctrl_state;

	BUG_ON(NULL == pctrl);

	pinctrl_state = pinctrl_lookup_state(pctrl, PINCTRL_STATE_IDLE);

	if (IS_ERR(pinctrl_state)) {
		pr_err("%s : could not get idle state (%li)\n",
				__FUNCTION__ , PTR_ERR(pinctrl_state));
		return -1;
	}

	if (pinctrl_select_state(pctrl, pinctrl_state)) {
		pr_err("%s : could not set pins to idle state\n", __FUNCTION__);
		return -1;
	}

	return 0;
}
#ifdef CONFIG_DEBUG_FS
static void hi6402es_page_dp(unsigned int page, char *buf)
{
	unsigned int regi, regcout;
	u32 *hi6402es_page;
	
	memset(buf, 0, HI6402ES_DBG_SIZE_PAGE);

	if (page > HI6402ES_DBG_PAGES) {
		snprintf(buf, HI6402ES_DBG_SIZE_PAGE, "error pages %d.\n", page);
		return;
	}

	snprintf(buf, HI6402ES_DBG_SIZE_PAGE, "Page %d\n", page);
	switch(page) {
	case 0:
		regcout = sizeof(hi6402es_page_reg0)/sizeof(u32);
		hi6402es_page= hi6402es_page_reg0;
		break;
	case 1:
		regcout = sizeof(hi6402es_page_reg1)/sizeof(u32);
		hi6402es_page= hi6402es_page_reg1;
		break;
	case 2:
		regcout = sizeof(hi6402es_page_reg2)/sizeof(u32);
		hi6402es_page= hi6402es_page_reg2;
		break;
	case 3:
		regcout = sizeof(hi6402es_page_reg3)/sizeof(u32);
		hi6402es_page= hi6402es_page_reg3;
		break;
	case 4:
		regcout = sizeof(hi6402es_page_reg4)/sizeof(u32);
		hi6402es_page= hi6402es_page_reg4;
		break;
	case 5:
		regcout = sizeof(hi6402es_page_reg5)/sizeof(u32);
		hi6402es_page= hi6402es_page_reg5;
		break;
	case 6:
		regcout = sizeof(hi6402es_page_reg6)/sizeof(u32);
		hi6402es_page= hi6402es_page_reg6;
		break;
	case 7:
		regcout = sizeof(hi6402es_page_reg7)/sizeof(u32);
		hi6402es_page= hi6402es_page_reg7;
		break;
	case 8:
		regcout = sizeof(hi6402es_page_reg8)/sizeof(u32);
		hi6402es_page= hi6402es_page_reg8;
		break;
	case 9:
		regcout = sizeof(hi6402es_page_reg9)/sizeof(u32);
		hi6402es_page= hi6402es_page_reg9;
		break;
	default:
		snprintf(buf, HI6402ES_DBG_SIZE_PAGE, "error pages %d.\n", page);
		return;
	}

	for (regi = regcout; regi != 0; regi --, hi6402es_page++) {
		snprintf(buf, HI6402ES_DBG_SIZE_PAGE, "%s%#08x:%#02x\n",
			buf, *hi6402es_page, hi6402es_irq_read(hi6402es_gpirq, *hi6402es_page));
	}

	return;
}

static ssize_t hi6402es_rr_read(struct file *file, char __user *user_buf,
			size_t count, loff_t *ppos)
{
	char *kn_buf = NULL;
	ssize_t byte_read = 0;
	struct hi6402es_ins m_hi6402es_ins;
	spin_lock(&hi6402es_ins_lock);
	m_hi6402es_ins = g_hi6402es_ins;
	spin_unlock(&hi6402es_ins_lock);

	kn_buf = kzalloc(HI6402ES_DBG_SIZE_PAGE, GFP_KERNEL);
	if (NULL == kn_buf) {
		pr_err("kn_buf is null\n");
		return simple_read_from_buffer(user_buf, count, ppos, "kn_buf alloc fail\n", 18);
	}

	switch(m_hi6402es_ins.ins) {
	case 'r':
		snprintf(kn_buf, HI6402ES_DBG_SIZE_PAGE, "%s %#08x:0x%x\n",
			kn_buf, m_hi6402es_ins.reg, hi6402es_irq_read(hi6402es_gpirq, m_hi6402es_ins.reg));
		break;
	case 'w':
		snprintf(kn_buf, HI6402ES_DBG_SIZE_PAGE, "write ok.\n");
		break;
	case 'p':
		hi6402es_page_dp(m_hi6402es_ins.val, kn_buf);
		break;
	case 'n':
	default:
		snprintf(kn_buf, HI6402ES_DBG_SIZE_PAGE, "%serror parameter.\n", kn_buf);
		break;
	}
	snprintf(kn_buf, HI6402ES_DBG_SIZE_PAGE, "%s<cat end>\n", kn_buf);
	snprintf(kn_buf, HI6402ES_DBG_SIZE_PAGE, "%s\n", kn_buf);

	byte_read = simple_read_from_buffer(user_buf, count, ppos, kn_buf, strlen(kn_buf));

	kfree(kn_buf);
	kn_buf = NULL;

	return byte_read;
}

static ssize_t hi6402es_rr_write(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos)
{
	char *kn_buf = NULL;
	ssize_t byte_writen = 0;
	int num = 0;
	struct hi6402es_ins m_hi6402es_ins;

	kn_buf = kzalloc(HI6402ES_DBG_SIZE_PAGE, GFP_KERNEL);
	if (NULL == kn_buf) {
		pr_err("kn_buf is null\n");
		return -EFAULT;
	}

	byte_writen = simple_write_to_buffer(kn_buf, HI6402ES_DBG_SIZE_PAGE, ppos, user_buf, count);

	switch(kn_buf[0]) {
	case 'w':
		m_hi6402es_ins.ins = 'w';
		num = sscanf(kn_buf, "w 0x%x 0x%x", &m_hi6402es_ins.reg, &m_hi6402es_ins.val);
		if (2 != num) {
			pr_err("%s:wrong parameter for cmd w.\n", __FUNCTION__);
			g_hi6402es_ins.ins = 'n';
			byte_writen = -EINVAL;
			break;
		}
		hi6402es_irq_write(hi6402es_gpirq, m_hi6402es_ins.reg, m_hi6402es_ins.val);
		spin_lock(&hi6402es_ins_lock);
		g_hi6402es_ins = m_hi6402es_ins;
		spin_unlock(&hi6402es_ins_lock);
		break;
	case 'r':
		m_hi6402es_ins.ins = 'r';
		num = sscanf(kn_buf, "r 0x%x", &m_hi6402es_ins.reg);
		if (1 != num) {
			pr_err("%s:wrong parameter for cmd r.\n", __FUNCTION__);
			byte_writen = -EINVAL;
			break;
		}
		spin_lock(&hi6402es_ins_lock);
		g_hi6402es_ins = m_hi6402es_ins;
		spin_unlock(&hi6402es_ins_lock);
		break;
	case 'p':
		m_hi6402es_ins.ins = 'p';
		num = sscanf(kn_buf, "p %d", &m_hi6402es_ins.val);
		if (1 != num) {
			pr_err("%s:wrong parameter for cmd r.\n", __FUNCTION__);
			byte_writen = -EINVAL;
			break;
		}
		spin_lock(&hi6402es_ins_lock);
		g_hi6402es_ins = m_hi6402es_ins;
		spin_unlock(&hi6402es_ins_lock);
		break;
	default:
		g_hi6402es_ins.ins = 'n';
		pr_err("%s:unknown cmd.\n", __FUNCTION__);
		byte_writen = -EINVAL;
		break;
	}

	kfree(kn_buf);
	kn_buf = NULL;

	return byte_writen;
}

static const struct file_operations hi6402es_rr_fops = {
	.read = hi6402es_rr_read,
	.write = hi6402es_rr_write,
};
#endif


static void hi6402es_irq_init_chip(struct hi6402es_irq *irq)
{
	BUG_ON(NULL == irq);

	/* ISO release */
	hi6402es_reg_set_bit(irq, HI6402ES_ISO_A18_D12_REG, HI6402ES_ISO_A18_D12_REG_BIT);
	hi6402es_reg_set_bit(irq, HI6402ES_ISO_A33_D12_REG, HI6402ES_ISO_A33_D12_REG_BIT);
}

static int hi6402es_irq_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	struct hi6402es_irq *irq = NULL;
	enum of_gpio_flags flags;
	unsigned int virq;
	int ret = 0;
	int i;

	irq = devm_kzalloc(dev, sizeof(*irq), GFP_KERNEL);
	if (!irq) {
		dev_err(dev, "cannot allocate hi6402es_irq device info\n");
		return -ENOMEM;
	}

	platform_set_drvdata(pdev, irq);

	/* get resources */
	irq->res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!irq->res) {
		dev_err(dev, "platform_get_resource err\n");
		goto err_exit;
	}

	if (!devm_request_mem_region(dev, irq->res->start,
				     resource_size(irq->res),
				     pdev->name)) {
		dev_err(dev, "cannot claim register memory\n");
		goto err_exit;
	}

	irq->reg_base_addr = devm_ioremap(dev, irq->res->start,
					  resource_size(irq->res));
	if (!irq->reg_base_addr) {
		dev_err(dev, "cannot map register memory\n");
		goto ioremap_err;
	}

	/* get pinctrl */
	irq->pctrl = devm_pinctrl_get(dev);
	if (IS_ERR(irq->pctrl)) {
		dev_err(dev, "could not get pinctrl\n");
		goto codec_ssi_get_err;
	}
	ret = codec_ssi_iomux_default(irq->pctrl);
	if (0 != ret)
		goto codec_ssi_iomux_err;

	/* get codec ssi clk */
	irq->codec_ssi_clk = devm_clk_get(dev, "clk_codecssi");
	if (IS_ERR(irq->codec_ssi_clk)) {
		pr_err("clk_get: codecssi clk not found!\n");
		ret = PTR_ERR(irq->codec_ssi_clk);
		goto codec_ssi_clk_err;
	}
	ret = clk_prepare_enable(irq->codec_ssi_clk);
	if (0 != ret) {
		pr_err("codec_ssi_clk :clk prepare enable failed !\n");
		goto codec_ssi_clk_enable_err;
	}

	irq->regu.supply = "hi6402es-irq";
	ret = devm_regulator_bulk_get(dev, 1, &(irq->regu));
	if (0 != ret) {
		dev_err(dev, "couldn't get regulators %d\n", ret);
		goto pmu_audio_clk_err;
	}
	/* enable ldo8 */
	ret = regulator_bulk_enable(1, &irq->regu);
	if (0 != ret) {
		pr_err("%s : couldn't enable regulators %d\n",
			__FUNCTION__, ret);
	}

	/* get pmu audio clk */
	irq->pmu_audio_clk = devm_clk_get(dev, "clk_pmuaudioclk");
	if (IS_ERR(irq->pmu_audio_clk)) {
		pr_err("_clk_get: pmu_audio_clk not found!\n");
		ret = PTR_ERR(irq->pmu_audio_clk);
		goto pmu_audio_clk_err;
	}
	ret = clk_prepare_enable(irq->pmu_audio_clk);
	if (0 != ret) {
		pr_err("pmu_audio_clk :clk prepare enable failed !\n");
		goto pmu_audio_clk_enable_err;
	}

	/* Ensure make a page change at first time. */
	irq->page_sel_ssi = 1;
	irq->ibias_work = 0;
	irq->ref_pll = 0;
	irq->pll_status = HI6402ES_PLL_PD;

	mutex_init(&irq->mutex);
	mutex_init(&irq->sr_mutex);
	mutex_init(&irq->ibias_mutex);
	mutex_init(&irq->pll_mutex);

	irq->dev = dev;

	/* check chip id */
	ret = hi6402es_irq_read(irq, HI6402ES_REG_VERSION);
	if (0xFF == ret || 0 == ret) {
		/*
		 * 0xFF : means ssi err, or chip no ack
		 * 0    : means no chip
		 */
		pr_err("no codec chip connected(%#x)!", ret);
		ret = -ENODEV;
		goto get_gpio_err;
	} else {
		pr_info("codec chip version is %#x", ret);
	}

	/* clear IRQ status */
	hi6402es_irq_write(irq, HI6402ES_REG_IRQM_0, 0xFF);
	hi6402es_irq_write(irq, HI6402ES_REG_IRQM_1, 0xFF);
	hi6402es_irq_write(irq, HI6402ES_REG_IRQM_2, 0xFF);
	hi6402es_irq_write(irq, HI6402ES_REG_IRQ_0, 0xFF);
	hi6402es_irq_write(irq, HI6402ES_REG_IRQ_1, 0xFF);
	hi6402es_irq_write(irq, HI6402ES_REG_IRQ_2, 0xFF);
	hi6402es_irq_write(irq, 0x20001000 + 0x284, 0x104);

	/* config irq iomux & control */
	hi6402es_reg_write_bits(irq, HI6402ES_IRQ_REG_IOMUX, 4, 4);
	hi6402es_irq_write(irq, HI6402ES_IRQ_REG_IRQ_CTRL, 0x84);

	irq->gpio = of_get_gpio_flags(np, 0, &flags);
	if (0 > irq->gpio) {
		dev_err(dev, "get gpio flags error\n");
		ret = irq->gpio;
		goto get_gpio_err;
	}

	if (!gpio_is_valid(irq->gpio)) {
		dev_err(dev, "gpio is invalid\n");
		ret = -EINVAL;
		goto get_gpio_err;
	}

	ret = gpio_request_one(irq->gpio, GPIOF_IN, "hi6402es_irq");
	if (0 > ret) {
		dev_err(dev, "failed to request gpio%d\n", irq->gpio);
		goto get_gpio_err;
	}

	irq->hi6402es_irq_delay_wq = create_singlethread_workqueue("hi6402es_irq_delay_wq");
	if (!(irq->hi6402es_irq_delay_wq)) {
		pr_err("%s(%u) : workqueue create failed", __FUNCTION__,__LINE__);
		ret = -ENOMEM;
		goto irq_delay_wq_err;
	}
	INIT_DELAYED_WORK(&irq->hi6402es_irq_delay_work, hi6402es_irq_work_func);

	irq->irq = gpio_to_irq(irq->gpio);

	irq->domain = irq_domain_add_simple(np, HI6402ES_MAX_IRQS, 0,
					    &hi6402es_domain_ops, irq);
	if (!irq->domain) {
		dev_err(dev, "irq domain error\n");
		ret = -ENODEV;
		goto gpio_err;
	}

	for (i = 0; i < HI6402ES_MAX_IRQS; i++) {
		virq = irq_create_mapping(irq->domain, i);
		if (virq == NO_IRQ) {
			dev_err(dev, "Failed mapping hwirq\n");
			ret = -ENOSPC;
			goto gpio_err;
		}
		irq->irqs[i] = virq;
	}

	ret = request_irq(irq->irq, hi6402es_irq_handler,
			IRQF_TRIGGER_LOW | IRQF_NO_SUSPEND,
			"hi6402es_irq", irq);
	if (0 > ret) {
		dev_err(dev, "could not claim irq %d\n", ret);
		ret = -ENODEV;
		goto gpio_err;
	}

	hi6402es_irq_init_chip(irq);

	/* populate sub nodes */
	of_platform_populate(np, of_hi6402es_irq_child_match_tbl, NULL, dev);

	wake_lock_init(&irq->wake_lock, WAKE_LOCK_SUSPEND, "hisi-6402-irq");

#ifdef CONFIG_DEBUG_FS
	debug_dir = debugfs_create_dir("hi6402", NULL);
	if (!debug_dir) {
		pr_err("anc_hs: Failed to create anc_hs debugfs dir\n");
		return 0;
	}
	if (!debugfs_create_file("rr", 0644, debug_dir, NULL, &hi6402es_rr_fops))
		pr_err("hi6402es: Failed to create hi6402es debugfs file\n");
	spin_lock_init(&hi6402es_ins_lock);
	hi6402es_gpirq = irq;
#endif

	pr_info("%s: init ok\n",__FUNCTION__);

	return 0;

gpio_err:
	gpio_free(irq->gpio);
irq_delay_wq_err:
	free_irq(irq->irq, irq);
get_gpio_err:
	clk_disable_unprepare(irq->pmu_audio_clk);
pmu_audio_clk_enable_err:
	devm_clk_put(dev, irq->pmu_audio_clk);
pmu_audio_clk_err:
	clk_disable_unprepare(irq->codec_ssi_clk);
codec_ssi_clk_enable_err:
	devm_clk_put(dev, irq->codec_ssi_clk);
codec_ssi_clk_err:
	codec_ssi_iomux_idle(irq->pctrl);
codec_ssi_iomux_err:
	pinctrl_put(irq->pctrl);
codec_ssi_get_err:
	devm_iounmap(dev, irq->reg_base_addr);
ioremap_err:
	devm_release_mem_region(dev, irq->res->start,
				resource_size(irq->res));
err_exit:
	devm_kfree(dev, irq);

	pr_err("%s: init failed\n",__FUNCTION__);

	return ret;
}

static int hi6402es_irq_remove(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct hi6402es_irq *irq = platform_get_drvdata(pdev);

#ifdef CONFIG_DEBUG_FS
	debugfs_remove_recursive(debug_dir);
#endif
	free_irq(irq->irq, irq);
	gpio_free(irq->gpio);
	clk_disable_unprepare(irq->pmu_audio_clk);
	devm_clk_put(dev, irq->pmu_audio_clk);
	clk_disable_unprepare(irq->codec_ssi_clk);
	devm_clk_put(dev, irq->codec_ssi_clk);
	codec_ssi_iomux_idle(irq->pctrl);
	pinctrl_put(irq->pctrl);
	devm_iounmap(dev, irq->reg_base_addr);
	devm_release_mem_region(dev, irq->res->start,
				resource_size(irq->res));
	devm_kfree(dev, irq);
	platform_set_drvdata(pdev, NULL);

	return 0;
}

static int hi6402es_irq_suspend(struct platform_device *pdev, pm_message_t state)
{
	struct device *dev = &pdev->dev;
	struct hi6402es_irq *irq = dev_get_drvdata(dev);
	int ret = 0;

	BUG_ON(NULL == irq);

	dev_info(dev, "%s+", __FUNCTION__);

	mutex_lock(&irq->sr_mutex);

	clk_disable_unprepare(irq->pmu_audio_clk);

	clk_disable_unprepare(irq->codec_ssi_clk);

	ret = codec_ssi_iomux_idle(irq->pctrl);
	if (0 != ret)
		dev_err(dev, "codec ssi set iomux idle err\n");

	dev_info(dev, "%s-", __FUNCTION__);

	return ret;
}

static int hi6402es_irq_resume(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct hi6402es_irq *irq = dev_get_drvdata(dev);
	int ret = 0;

	BUG_ON(NULL == irq);

	ret = codec_ssi_iomux_default(irq->pctrl);
	if (0 != ret)
		goto err_exit;

	ret = clk_prepare_enable(irq->codec_ssi_clk);
	if (0 != ret) {
		dev_err(dev, "codec_ssi_clk :clk prepare enable failed !\n");
		goto err_exit;
	}

	ret = clk_prepare_enable(irq->pmu_audio_clk);
	if (0 != ret) {
		dev_err(dev, "pmu_audio_clk :clk prepare enable failed !\n");
		goto err_exit;
	}
err_exit:
	dev_info(dev, "%s-", __FUNCTION__);

	mutex_unlock(&irq->sr_mutex);

	return ret;
}

static struct platform_driver hi6402es_irq_driver = {
	.driver = {
		.name	= "hi6402es_irq",
		.owner  = THIS_MODULE,
		.of_match_table = of_hi6402es_irq_match,
	},
	.probe		= hi6402es_irq_probe,
	.remove		= hi6402es_irq_remove,
	.suspend	= hi6402es_irq_suspend,
	.resume		= hi6402es_irq_resume,
};

static int __init hi6402es_irq_init(void)
{
	return platform_driver_register(&hi6402es_irq_driver);
}

static void __exit hi6402es_irq_exit(void)
{
	platform_driver_unregister(&hi6402es_irq_driver);
}

fs_initcall(hi6402es_irq_init);
module_exit(hi6402es_irq_exit);

MODULE_AUTHOR("chengong <apollo.chengong@hisilicon.com>");
MODULE_DESCRIPTION("Hisi 6402 irq driver");
MODULE_LICENSE("GPL v2");
