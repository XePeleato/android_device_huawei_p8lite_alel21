/*
 * Copyright (c) 2013 Linaro Ltd.
 * Copyright (c) 2013 Hisilicon Limited.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
/*lint --e{750}*/
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/sd.h>
#include <linux/mmc/sdio.h>
#include <linux/mmc/host.h>
#include <linux/mmc/dw_mmc.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/pinctrl/consumer.h>
#include <linux/regulator/consumer.h>
#include <linux/of_address.h>
#include <linux/pm_runtime.h>
#include <linux/clk-provider.h>

#include "dw_mmc.h"
#include "dw_mmc-pltfm.h"

#ifdef CONFIG_HUAWEI_EMMC_DSM
#include <linux/mmc/dsm_emmc.h>
#endif
#ifdef CONFIG_HUAWEI_DSM
#include <huawei_platform/dsm/dsm_pub.h>

static struct dsm_dev dsm_dw_mmc = {
	.name = "dsm_sdio",
	.device_name = NULL,
	.ic_name = NULL,
	.module_name = NULL,
	.fops = NULL,
	.buff_size = 2048,
};
static struct dsm_client *dclient = NULL;
#endif

#define DRIVER_NAME "dwmmc_hs"

#define DW_MCI_EMMC_ID    (0x00)
#define DW_MCI_SD_ID      (0x01)
#define DW_MCI_SDIO_ID    (0x02)

#define PERI_CRG_RSTEN4   (0x90)
#define PERI_CRG_RSTDIS4  (0x94)
/*#define PERI_CRG_RSTSTAT4 (0x98)*//*not used*/

#define BIT_RST_EMMC      (1<<17)
#define BIT_RST_SD        (1<<18)
#define BIT_RST_SDIO      (1<<19)

#define GPIO_CLK_ENABLE (0x1 << 16)
#define UHS_REG_EXT_SAMPLE_PHASE(x) (((x) & 0x1f) << 16)
#define UHS_REG_EXT_SAMPLE_DLY(x) (((x) & 0x1f) << 23)
#define SDMMC_UHS_REG_EXT_VALUE(x, y)                                          \
	(UHS_REG_EXT_SAMPLE_PHASE(x) | UHS_REG_EXT_SAMPLE_DLY(y))

#define GPIO_CLK_DIV(x) (((x) & 0xf) << 8)
#define GPIO_USE_SAMPLE_DLY(x) (((x) & 0x1) << 13)
#define GPIO_DRIVE_SEL(x) (((x) & 0x1) << 12)
#define SDMMC_GPIO_VALUE(x, y, z)                                              \
	(GPIO_CLK_DIV(x) | GPIO_USE_SAMPLE_DLY(y) | GPIO_DRIVE_SEL(z))

static void __iomem *pericrg_base = NULL;
static void __iomem *sys_base = NULL;

static unsigned long hs_dwmmc_caps[];

#define MAX_TUNING_LOOP 200
static const u8 tuning_blk_pattern_4bit[] = {
        0xff, 0x0f, 0xff, 0x00, 0xff, 0xcc, 0xc3, 0xcc,
        0xc3, 0x3c, 0xcc, 0xff, 0xfe, 0xff, 0xfe, 0xef,
        0xff, 0xdf, 0xff, 0xdd, 0xff, 0xfb, 0xff, 0xfb,
        0xbf, 0xff, 0x7f, 0xff, 0x77, 0xf7, 0xbd, 0xef,
        0xff, 0xf0, 0xff, 0xf0, 0x0f, 0xfc, 0xcc, 0x3c,
        0xcc, 0x33, 0xcc, 0xcf, 0xff, 0xef, 0xff, 0xee,
        0xff, 0xfd, 0xff, 0xfd, 0xdf, 0xff, 0xbf, 0xff,
        0xbb, 0xff, 0xf7, 0xff, 0xf7, 0x7f, 0x7b, 0xde,
};

static const u8 tuning_blk_pattern_8bit[] = {
        0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00,
        0xff, 0xff, 0xcc, 0xcc, 0xcc, 0x33, 0xcc, 0xcc,
        0xcc, 0x33, 0x33, 0xcc, 0xcc, 0xcc, 0xff, 0xff,
        0xff, 0xee, 0xff, 0xff, 0xff, 0xee, 0xee, 0xff,
        0xff, 0xff, 0xdd, 0xff, 0xff, 0xff, 0xdd, 0xdd,
        0xff, 0xff, 0xff, 0xbb, 0xff, 0xff, 0xff, 0xbb,
        0xbb, 0xff, 0xff, 0xff, 0x77, 0xff, 0xff, 0xff,
        0x77, 0x77, 0xff, 0x77, 0xbb, 0xdd, 0xee, 0xff,
        0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00,
        0x00, 0xff, 0xff, 0xcc, 0xcc, 0xcc, 0x33, 0xcc,
        0xcc, 0xcc, 0x33, 0x33, 0xcc, 0xcc, 0xcc, 0xff,
        0xff, 0xff, 0xee, 0xff, 0xff, 0xff, 0xee, 0xee,
        0xff, 0xff, 0xff, 0xdd, 0xff, 0xff, 0xff, 0xdd,
        0xdd, 0xff, 0xff, 0xff, 0xbb, 0xff, 0xff, 0xff,
        0xbb, 0xbb, 0xff, 0xff, 0xff, 0x77, 0xff, 0xff,
        0xff, 0x77, 0x77, 0xff, 0x77, 0xbb, 0xdd, 0xee,
};

static int hs_timing_config[][9][TUNING_INIT_CONFIG_NUM] = {
/* bus_clk, div, drv_sel, sam_dly, sam_phase_max, sam_phase_min, input_clk */
	{ { 19200000   , 11 , 1 , 0 , 23 , 23 , 1600000 }   , /* 0: LEGACY 400k */
	  { 360000000  , 6  , 1 , 0 , 1  , 1  , 52000000 }  , /* 1: MMC_HS*/
	  { 0 },                                              /* 2: SD_HS */
	  { 0 },                                              /* 3: SDR12 */
	  { 0 },                                              /* 4: SDR25 */
	  { 0 },                                              /* 5: SDR50 */
	  { 0 },                                              /* 6: SDR104 */
	  { 720000000  , 6  , 0 , 4 , 6  , 0  , 102000000 } , /* 7: DDR50 */
	  { 1440000000 , 7  , 0 , 2 , 15 , 0  , 180000000 } , /* 8: HS200 */
	}              ,
	{ { 90000000   , 15 , 1 , 0 , 31 , 31 , 5600000 }   , /* 0: LEGACY 400k */
	  { 0 },                                              /* 1: MMC_HS */
	  { 360000000  , 6  , 1 , 0 , 1  , 1  , 50000000 }  , /* 2: SD_HS */
	  { 180000000  , 6  , 1 , 2 , 13 , 13 , 25000000 }  , /* 3: SDR12 */
	  { 360000000  , 6  , 1 , 0 , 1  , 1  , 50000000 }  , /* 4: SDR25 */
	  { 720000000  , 6  , 0 , 3 , 13 , 0  , 100000000 } , /* 5: SDR50 */
	  { 1440000000 , 6  , 0 , 3 , 13 , 0  , 200000000 } , /* 6: SDR104 */
	  { 720000000  , 15 , 0 , 5 , 11 , 1  , 45000000 }  , /* 7: DDR50 */
	  { 0 },                                              /* 8: HS200 */
	}              ,
	{ { 90000000   , 15 , 1 , 0 , 31 , 31 , 5600000 }   , /* 0: LEGACY 400k */
	  { 0 },                                              /* 1: MMC_HS */
	  { 360000000  , 6  , 1 , 0 , 1  , 1  , 50000000 }  , /* 2: SD_HS, same as SDR25 */
	  { 180000000  , 6  , 1 , 2 , 13 , 13 , 25000000 }  , /* 3: SDR12 */
	  { 360000000  , 6  , 1 , 0 , 1  , 1  , 50000000 }  , /* 4: SDR25 */
	  { 720000000  , 6  , 0 , 2 , 9  , 1  , 100000000 } , /* 5: SDR50 */
	  { 1440000000 , 7  , 0 , 3 , 15 , 0  , 180000000 } , /* 6: SDR104 */
	  { 720000000  , 15 , 0 , 5 , 11 , 1  , 45000000 }  , /* 7: DDR50 */
	  { 0 },                                              /* 8: HS200 */
	}
};

static void dw_mci_hs_set_timing(struct dw_mci *host, int id, int timing,
				 int sam_phase, int clk_div)
{
	int cclk_div;
	int drive_sel;
	int sam_dly;
	int sam_phase_max, sam_phase_min;
	int sam_phase_val;
	int reg_value;
	int enable_shift = 0;
	int use_sam_dly = 0;
	int d_value = 0;

	if ((host->hw_mmc_id == DWMMC_SD_ID) && (timing == MMC_TIMING_LEGACY))
		cclk_div = hs_timing_config[id][timing][1];
	else
		cclk_div = clk_div;
	if (host->hw_mmc_id == DWMMC_SD_ID) {
		d_value = cclk_div - hs_timing_config[id][timing][1];
	}
	drive_sel = hs_timing_config[id][timing][2];
	sam_dly = hs_timing_config[id][timing][3] + d_value;
	sam_phase_max = hs_timing_config[id][timing][4] + 2 * d_value;
	sam_phase_min = hs_timing_config[id][timing][5];

	if (sam_phase == -1)
		sam_phase_val = (sam_phase_max + sam_phase_min) / 2;
	else
		sam_phase_val = sam_phase;

	/* enable_shift and use_sam_dly setting code */
	/* warning! different with K3V3 */
	switch(id){
	case DW_MCI_EMMC_ID:
		switch(timing){
		case MMC_TIMING_UHS_DDR50:
			if(4 <= sam_phase_val && sam_phase_val <= 6)
				enable_shift = 1;
			break;
		case MMC_TIMING_MMC_HS200:
			if(4 <= sam_phase_val && sam_phase_val <= 14)
				enable_shift = 1;
			if(9 <= sam_phase_val && sam_phase_val <= 14)
				use_sam_dly = 1;
			break;
		}
		break;
	case DW_MCI_SD_ID:
		switch(timing){
		case MMC_TIMING_UHS_SDR12:
			use_sam_dly = 1;
			break;
		case MMC_TIMING_UHS_DDR50:
			if(4 <= sam_phase_val && sam_phase_val <= 11)
				enable_shift = 1;
			break;
		case MMC_TIMING_UHS_SDR50:
			if (10 + 2 * d_value <= sam_phase_val &&
			    sam_phase_val <= 12 + 2 * d_value)
				use_sam_dly = 1;
			if (4 + d_value <= sam_phase_val &&
			    sam_phase_val <= 12 + d_value)
				enable_shift = 1;
			break;
		case MMC_TIMING_UHS_SDR104:
			if (8 + 2 * d_value <= sam_phase_val &&
			    sam_phase_val <= 12 + 2 * d_value)
				use_sam_dly = 1;
			if (4 + d_value <= sam_phase_val &&
			    sam_phase_val <= 12 + d_value)
				enable_shift = 1;
			break;
		}
		break;
	case DW_MCI_SDIO_ID:
		switch(timing){
		case MMC_TIMING_UHS_SDR12:
			use_sam_dly = 1;
			break;
		case MMC_TIMING_UHS_DDR50:
			if(4 <= sam_phase_val && sam_phase_val <= 11)
				enable_shift = 1;
			break;
		case MMC_TIMING_UHS_SDR50:
			if(4 <= sam_phase_val && sam_phase_val <= 9)
				enable_shift = 1;
			break;
		case MMC_TIMING_UHS_SDR104:
			if(9 <= sam_phase_val && sam_phase_val <= 14)
				use_sam_dly = 1;
			if(4 <= sam_phase_val && sam_phase_val <= 14)
				enable_shift = 1;
			break;
		}
		break;
	}


	/*
	if (timing == MMC_TIMING_LEGACY)
		enable_shift = 1;

	if ((id == 0) && (timing == MMC_TIMING_MMC_HS200)) {
		switch (sam_phase_val) {
		case 0:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			use_sam_dly = 1;
			break;
		default:
			use_sam_dly = 0;
		}
	}

	if ((id == 1) && (timing == MMC_TIMING_UHS_SDR104)) {
		if ((sam_phase_val == 0) || (sam_phase_val >= 10))
			use_sam_dly = 1;
	}

	if ((id == 2) && (timing == MMC_TIMING_UHS_SDR104)) {
		switch (sam_phase_val) {
		case 0:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			use_sam_dly = 1;
			break;
		default:
			use_sam_dly = 0;
		}
	}
	*/

	/* first disabl clk */
	/* mci_writel(host, GPIO, ~GPIO_CLK_ENABLE); */
	mci_writel(host, GPIO, 0x0);
	udelay(1);

	reg_value = SDMMC_UHS_REG_EXT_VALUE(sam_phase_val, sam_dly);
	mci_writel(host, UHS_REG_EXT, reg_value);

	mci_writel(host, ENABLE_SHIFT, enable_shift);

	reg_value = SDMMC_GPIO_VALUE(cclk_div, use_sam_dly, drive_sel);
	mci_writel(host, GPIO, reg_value | GPIO_CLK_ENABLE);

	dev_info(
	    host->dev,
	    "id=%d,timing=%d,UHS_REG_EXT=0x%x,ENABLE_SHIFT=0x%x,GPIO=0x%x\n",
	    id, timing, mci_readl(host, UHS_REG_EXT),
	    mci_readl(host, ENABLE_SHIFT), mci_readl(host, GPIO));
}

static void dw_mci_hs_set_parent(struct dw_mci *host, int timing)
{
	struct dw_mci_hs_priv_data *priv = host->priv;
	int id = priv->id;
	struct clk *clk_parent = NULL;
	int reg_value;

	if (id != DW_MCI_EMMC_ID)
		return;

	/* first disable GPIO div clock */
	reg_value = mci_readl(host, GPIO);
	mci_writel(host, GPIO, reg_value & ~GPIO_CLK_ENABLE);
	udelay(1);

	/* select 19.2M clk */
	if (timing == MMC_TIMING_LEGACY) {
		clk_parent = clk_get_parent_by_index(host->parent_clk, 0);
		if (IS_ERR_OR_NULL(clk_parent)) {
			dev_err(host->dev, " fail to get parent clk. \n");
		}

		clk_set_parent(host->parent_clk, clk_parent);
	} else {
		clk_parent = clk_get_parent_by_index(host->parent_clk, 4);
		if (IS_ERR_OR_NULL(clk_parent)) {
			dev_err(host->dev, " fail to get parent clk. \n");
		}

		clk_set_parent(host->parent_clk, clk_parent);
	}

	/* enable internal GPIO div clock */
	mci_writel(host, GPIO, reg_value | GPIO_CLK_ENABLE);

	/*
	clk_parent = clk_get_parent_by_index(host->parent_clk, 4);
	if (IS_ERR_OR_NULL(clk_parent)) {
		dev_err(host->dev, " fail to get parent clk. \n");
	}

	clk_set_parent(host->parent_clk, clk_parent);
	*/
}

#ifdef CONFIG_HUAWEI_DSM
 void dw_mci_dsm_dump(struct dw_mci  *host, int err_num)
{
	u32 status, mintsts;
	if (dclient == NULL){
		printk(KERN_ERR "dclient is not initialization\n");
		return;
	}
	if(host== NULL) {
		printk(KERN_ERR "sdio host NULL.\n");
		return;
	}
	dev_err(host->dev, "dsm_sido err_num = %d \n", err_num);
	if(!dsm_client_ocuppy(dclient)){
		dsm_client_record(dclient, "CTRL:0x%x\n", mci_readl(host, CTRL));
		dsm_client_record(dclient, "PWREN:0x%x\n", mci_readl(host, PWREN));
		dsm_client_record(dclient, "CLKDIV:0x%x\n", mci_readl(host, CLKDIV));
		dsm_client_record(dclient, "CLKSRC:0x%x\n", mci_readl(host, CLKSRC));
		dsm_client_record(dclient, "CLKENA:0x%x\n", mci_readl(host, CLKENA));
		dsm_client_record(dclient, "TMOUT:0x%x\n", mci_readl(host, TMOUT));
		dsm_client_record(dclient, "CTYPE:0x%x\n", mci_readl(host, CTYPE));
		dsm_client_record(dclient, "BLKSIZ:0x%x\n", mci_readl(host, BLKSIZ));
		dsm_client_record(dclient, "BYTCNT:0x%x\n", mci_readl(host, BYTCNT));
		dsm_client_record(dclient, "INTMSK:0x%x\n", mci_readl(host, INTMASK));
		dsm_client_record(dclient, "CMDARG:0x%x\n", mci_readl(host, CMDARG));
		dsm_client_record(dclient, "CMD:0x%x\n", mci_readl(host, CMD));
		dsm_client_record(dclient, "MINTSTS:0x%x\n", mci_readl(host, MINTSTS));
		dsm_client_record(dclient, "RINTSTS:0x%x\n", mci_readl(host, RINTSTS));
		dsm_client_record(dclient, "STATUS:0x%x\n", mci_readl(host, STATUS));
		dsm_client_record(dclient, "FIFOTH:0x%x\n", mci_readl(host, FIFOTH));
		dsm_client_record(dclient, "CDETECT:0x%x\n", mci_readl(host, CDETECT));
		dsm_client_record(dclient, "WRTPRT:0x%x\n", mci_readl(host, WRTPRT));
		dsm_client_record(dclient, "GPIO:0x%x\n", mci_readl(host, GPIO));
		dsm_client_record(dclient, "TCBCNT:0x%x\n", mci_readl(host, TCBCNT));
		dsm_client_record(dclient, "TBBCNT:0x%x\n", mci_readl(host, TBBCNT));
		dsm_client_record(dclient, "DEBNCE:0x%x\n", mci_readl(host, DEBNCE));
		dsm_client_record(dclient, "USRID:0x%x\n", mci_readl(host, USRID));
		dsm_client_record(dclient, "VERID:0x%x\n", mci_readl(host, VERID));
		dsm_client_record(dclient, "HCON:0x%x\n", mci_readl(host, HCON));
		dsm_client_record(dclient, "UHS_REG:0x%x\n", mci_readl(host, UHS_REG));
		dsm_client_record(dclient, "BMOD:0x%x\n", mci_readl(host, BMOD));
		dsm_client_record(dclient, "PLDMND:0x%x\n", mci_readl(host, PLDMND));
		dsm_client_record(dclient, "DBADDR:0x%x\n", mci_readl(host, DBADDR));
		dsm_client_record(dclient, "IDSTS:0x%x\n", mci_readl(host, IDSTS));
		dsm_client_record(dclient, "IDINTEN:0x%x\n", mci_readl(host, IDINTEN));
		dsm_client_record(dclient, "DSCADDR:0x%x\n", mci_readl(host, DSCADDR));
		dsm_client_record(dclient, "BUFADDR:0x%x\n", mci_readl(host, BUFADDR));
		dsm_client_record(dclient, "CARDTHRCTL:0x%x\n", mci_readl(host, CARDTHRCTL));
		dsm_client_record(dclient, "UHS_REG_EXT:0x%x\n", mci_readl(host, UHS_REG_EXT));
		dsm_client_record(dclient, "cmd_status:0x%x\n", host->cmd_status);
		dsm_client_record(dclient, "data_status:0x%x\n", host->data_status);
		dsm_client_record(dclient, "pending_events:0x%x\n", host->pending_events);
		dsm_client_record(dclient, "completed_events:0x%x\n", host->completed_events);
		dsm_client_record(dclient, "state:%d\n", host->state);
		/* summary */
		dsm_client_record(dclient, "MINTSTS = %d\n", mci_readl(host, MINTSTS));
		dsm_client_record(dclient, "STATUS = %d\n", mci_readl(host, STATUS));
		dsm_client_record(dclient, "CMD=%d\n",mci_readl(host, CMD));
		dsm_client_record(dclient, "ARG=0x%x \n",mci_readl(host, CMDARG));
		dsm_client_record(dclient, "RESP0:0x%x\n", mci_readl(host, RESP0));
		dsm_client_record(dclient, "RESP1:0x%x\n", mci_readl(host, RESP1));
		dsm_client_record(dclient, "RESP2:0x%x\n", mci_readl(host, RESP2));
		dsm_client_record(dclient, "RESP3:0x%x\n", mci_readl(host, RESP3));
		dsm_client_record(dclient, "host :cmd_status=0x%x.\n", host->cmd_status);
		dsm_client_record(dclient, "data_status=0x%x.\n", host->data_status);
		dsm_client_record(dclient, "host:pending_events=0x%x\n", host->pending_events);
		dsm_client_record(dclient,"completed_events=0x%x.\n", host->completed_events);

		dsm_client_notify(dclient, err_num);
    }else
		printk("DSM CALL FAIL, MCI  ID: %d\n", host->hw_mmc_id);
}

EXPORT_SYMBOL(dw_mci_dsm_dump);
#endif
static void dw_mci_hs_set_ios(struct dw_mci *host, struct mmc_ios *ios)
{
	struct dw_mci_hs_priv_data *priv = host->priv;
	int id = priv->id;
	int ret;

	if (priv->old_power_mode != ios->power_mode) {
		switch (ios->power_mode) {
		case MMC_POWER_OFF:
			dev_info(host->dev, "set io to lowpower\n");

			/* set pin to idle, skip emmc for vccq keeping power always on */
			if (host->hw_mmc_id != DWMMC_EMMC_ID){
				if ((host->pinctrl) && (host->pins_idle)) {
					ret = pinctrl_select_state(host->pinctrl,
								   host->pins_idle);
					if (ret)
						dev_warn(host->dev,
							 "could not set idle pins\n");
				}
			}

			if (host->vqmmc)
				regulator_disable(host->vqmmc);

			if (host->vmmc)
				regulator_disable(host->vmmc);

			break;
		case MMC_POWER_UP:
			dev_info(host->dev, "set io to normal\n");

			if(host->hw_mmc_id != DWMMC_SD_ID) {
				if ((host->pinctrl) && (host->pins_default)) {
					ret = pinctrl_select_state(host->pinctrl,
								   host->pins_default);
					if (ret)
						dev_warn(
						    host->dev,
						    "could not set default pins\n");
				}
			}

			if (host->vmmc) {
				ret = regulator_set_voltage(host->vmmc, 2950000,
							    2950000);
				if (ret)
					dev_err(
					    host->dev,
					    "regulator_set_voltage failed !\n");

				ret = regulator_enable(host->vmmc);
				if (ret)
					dev_err(host->dev,
						"regulator_enable failed !\n");
			}

			if (host->vqmmc) {
				ret = regulator_set_voltage(host->vqmmc,
							    2950000, 2950000);
				if (ret)
					dev_err(
					    host->dev,
					    "regulator_set_voltage failed !\n");

				ret = regulator_enable(host->vqmmc);
				if (ret)
					dev_err(host->dev,
						"regulator_enable failed !\n");
			}

			if(host->hw_mmc_id == DWMMC_SD_ID) {

				/* Wait for 500us~1ms to avoid semi-high level of sdcard io ports */
				usleep_range(500, 1000);

				if ((host->pinctrl) && (host->pins_default)) {
					ret = pinctrl_select_state(host->pinctrl,
								   host->pins_default);
					if (ret)
						dev_warn(
						    host->dev,
						    "could not set default pins\n");
				}
			}

			break;
		case MMC_POWER_ON:
			break;
		default:
			dev_info(host->dev, "unknown power supply mode\n");
			break;
		}
		priv->old_power_mode = ios->power_mode;
	}

	if (priv->old_timing != ios->timing) {

		dw_mci_hs_set_parent(host, ios->timing);

		ret = clk_set_rate(host->ciu_clk,
				   hs_timing_config[id][ios->timing][0]);
		if (ret)
			dev_err(host->dev, "clk_set_rate failed\n");

		host->tuning_init_sample =
		    (hs_timing_config[id][ios->timing][4] +
		     hs_timing_config[id][ios->timing][5]) /
		    2;

		if (host->sd_reinit == 0)
			host->current_div =
			    hs_timing_config[id][ios->timing][1];

		dw_mci_hs_set_timing(host, id, ios->timing,
				     host->tuning_init_sample,
				     host->current_div);

		if (priv->priv_bus_hz == 0)
			host->bus_hz = hs_timing_config[id][ios->timing][6];
		else
			host->bus_hz = 2 * hs_timing_config[id][ios->timing][6];

		priv->old_timing = ios->timing;
	}
}

static void dw_mci_hs_prepare_command(struct dw_mci *host, u32 *cmdr)
{
	*cmdr |= SDMMC_CMD_USE_HOLD_REG;
}

static int dw_mci_hs_get_resource(void)
{
	struct device_node *np = NULL;

	if (!pericrg_base) {
		np = of_find_compatible_node(NULL, NULL, "Hisilicon,crgctrl");
		if (!np) {
			printk("can't find crgctrl!\n");
			return -1;
		}

		pericrg_base = of_iomap(np, 0);
		if (!pericrg_base) {
			printk("crgctrl iomap error!\n");
			return -1;
		}
	}

	if (!sys_base) {
		np = of_find_compatible_node(NULL, NULL, "Hisilicon,sysctrl");
		if (!np) {
			printk("can't find sysctrl!\n");
			return -1;
		}

		sys_base = of_iomap(np, 0);
		if (!sys_base) {
			printk("sysctrl iomap error!\n");
			return -1;
		}
	}

	return 0;
}

/******************************************************************************
 * Do private setting specified for controller.
 * dw_mci_hs_priv_init execute before controller unreset,
 * this will cause NOC error.
 * put this function after unreset and clock set.
 *****************************************************************************/
static void dw_mci_hs_priv_setting(struct dw_mci *host)
{
	/* set threshold to 512 bytes */
	mci_writel(host, CARDTHRCTL, 0x02000001);
}

static int dw_mci_hs_set_controller(struct dw_mci *host, bool set)
{
	struct dw_mci_hs_priv_data *priv = host->priv;
	int id = priv->id;

	if (pericrg_base == NULL) {
		dev_err(host->dev, "pericrg_base is null, can't reset mmc! \n");
		return -1;
	}

	if (set) {
		if (0 == id) {
			writel(BIT_RST_EMMC, pericrg_base + PERI_CRG_RSTEN4);
			dev_info(host->dev, "rest emmc \n");
			goto out;
		} else if (1 == id) {
			writel(BIT_RST_SD, pericrg_base + PERI_CRG_RSTEN4);
			dev_info(host->dev, "rest sd \n");
			goto out;
		} else if (2 == id) {
			writel(BIT_RST_SDIO, pericrg_base + PERI_CRG_RSTEN4);
			dev_info(host->dev, "rest sdio \n");
			goto out;
		}
	} else {
		if (0 == id) {
			writel(BIT_RST_EMMC, pericrg_base + PERI_CRG_RSTDIS4);
			dev_info(host->dev, "unrest emmc \n");
			goto out;
		} else if (1 == id) {
			writel(BIT_RST_SD, pericrg_base + PERI_CRG_RSTDIS4);
			dev_info(host->dev, "unrest sd \n");
			goto out;
		} else if (2 == id) {
			writel(BIT_RST_SDIO, pericrg_base + PERI_CRG_RSTDIS4);
			dev_info(host->dev, "unrest sdio \n");
			goto out;
		}

	}
out:
	return 0;
}

#ifdef CONFIG_BCMDHD
extern struct dw_mci* sdio_host;

void dw_mci_sdio_card_detect(struct dw_mci *host)
{
        if(host == NULL){
                printk(KERN_ERR "sdio detect, host is null,can not used to detect sdio\n");
                return;
        }

        dw_mci_set_cd(host);

        queue_work(host->card_workqueue, &host->card_work);
        return;
};
#endif

static int dw_mci_hs_priv_init(struct dw_mci *host)
{
    struct dw_mci_hs_priv_data *priv;
    int error = -1;

    priv = devm_kzalloc(host->dev, sizeof(*priv), GFP_KERNEL);
    if (!priv) {
        dev_err(host->dev, "mem alloc failed for private data\n");
        return -ENOMEM;
    }
    priv->id = of_alias_get_id(host->dev->of_node, "mshc");
    priv->old_timing = -1;
    host->priv = priv;
    host->hw_mmc_id = priv->id;
    host->flags &= ~DWMMC_IN_TUNING;
    host->flags &= ~DWMMC_TUNING_DONE;

#ifdef CONFIG_BCMDHD
    if (priv->id == DW_MCI_SDIO_ID) {
        sdio_host = host;
    }
#endif

    switch(priv->id)
    {
        case MMC_EMMC:
            error = dev_set_name(host->dev, "hi_mci.0");
            if(error < 0)
            {
                dev_err(host->dev, "dev set name hi_mci.0 fail \n");
                goto fail;
            }

            break;
        case MMC_SD:
            error = dev_set_name(host->dev, "hi_mci.1");
            if(error < 0)
            {
                dev_err(host->dev, "dev set name hi_mci.1 fail \n");
                goto fail;
            }

            break;
        case MMC_SDIO:
            error = dev_set_name(host->dev, "hi_mci.2");
            if(error < 0)
            {
                dev_err(host->dev, "dev set name hi_mci.2 fail \n");
                goto fail;
            }

            break;
        default:
            dev_err(host->dev, "mpriv->id is out of range!!! \n");
			goto fail;
            break;
    }

    return 0;

fail:
    devm_kfree(host->dev,priv);
    return -1;
}

static int dw_mci_hs_setup_clock(struct dw_mci *host)
{
	struct dw_mci_hs_priv_data *priv = host->priv;
	int timing = MMC_TIMING_LEGACY;
	int id = priv->id;
	int ret;

	dw_mci_hs_set_parent(host, timing);

	ret = clk_set_rate(host->ciu_clk, hs_timing_config[id][timing][0]);
	if (ret)
		dev_err(host->dev, "clk_set_rate failed\n");

	dw_mci_hs_set_controller(host, 0);
	dw_mci_hs_priv_setting(host);

	host->tuning_current_sample = -1;
	host->sd_reinit = 0;
	host->current_div = hs_timing_config[id][timing][1];

	host->tuning_init_sample = (hs_timing_config[id][timing][4] +
				    hs_timing_config[id][timing][5]) / 2;

	dw_mci_hs_set_timing(host, id, timing, host->tuning_init_sample,
			     host->current_div);

	if (priv->priv_bus_hz == 0)
		host->bus_hz = hs_timing_config[id][timing][6];
	else
		host->bus_hz = priv->priv_bus_hz;

	priv->old_timing = timing;

	return 0;
}

static int dw_mci_hs_parse_dt(struct dw_mci *host)
{
	struct dw_mci_hs_priv_data *priv = host->priv;
	struct device_node *np = host->dev->of_node;
	int value = 0;

	if (of_property_read_u32(np, "hisi,bus_hz", &value)) {
		dev_info(host->dev, "bus_hz property not found, using "
				    "value of 0 as default\n");
		value = 0;
	}
	priv->priv_bus_hz = value;
	dev_info(host->dev, "dts bus_hz = %d \n", priv->priv_bus_hz);

	value = 0;
	if (of_property_read_u32(np, "cd-vol", &value)) {
		dev_info(host->dev, "cd-vol property not found, using "
				    "value of 0 as default\n");
		value = 0;
	}
	priv->cd_vol = value;
	dev_info(host->dev, "dts cd-vol = %d \n", priv->cd_vol);

	if (of_find_property(np, "sdio_support_uhs", NULL))
		hs_dwmmc_caps[DW_MCI_SDIO_ID] |=
		    (MMC_CAP_UHS_SDR12 | MMC_CAP_UHS_SDR25 | MMC_CAP_UHS_SDR50 |
		     MMC_CAP_UHS_SDR104);

	return 0;
}

static irqreturn_t dw_mci_hs_card_detect(int irq, void *data)
{
	struct dw_mci *host = (struct dw_mci *)data;
	host->sd_reinit = 0;
	host->flags &= ~DWMMC_IN_TUNING;
	host->flags &= ~DWMMC_TUNING_DONE;

	queue_work(host->card_workqueue, &host->card_work);
	return IRQ_HANDLED;
};

static int dw_mci_hs_get_cd(struct dw_mci *host, u32 slot_id)
{
	unsigned int status;
	struct dw_mci_hs_priv_data *priv = host->priv;

	/* cd_vol = 1 means sdcard gpio detect pin active-high */
	if (priv->cd_vol)
		status = !gpio_get_value(priv->gpio_cd);
	else /* cd_vol = 0 means sdcard gpio detect pin active-low */
		status = gpio_get_value(priv->gpio_cd);

	dev_info(host->dev, " sd status = %d\n", status);
	return status;
}

static int dw_mci_hs_cd_detect_init(struct dw_mci *host)
{
	struct device_node *np = host->dev->of_node;
	int gpio;
	int err;

	if (host->pdata->quirks & DW_MCI_QUIRK_BROKEN_CARD_DETECTION)
		return 0;

	gpio = of_get_named_gpio(np, "cd-gpio", 0);
	if (gpio_is_valid(gpio)) {
		if (devm_gpio_request_one(host->dev, gpio, GPIOF_IN, "dw-mci-cd")) {
			dev_err(host->dev, "gpio [%d] request failed\n", gpio);
		} else {
			struct dw_mci_hs_priv_data *priv = host->priv;
			int shared_irq = 0;
			priv->gpio_cd = gpio;
			host->pdata->get_cd = dw_mci_hs_get_cd;
			if (of_property_read_u32(np, "shared-irq", &shared_irq))
			{
				dev_info(host->dev, "shared-irq property not found, using "
					"shared_irq of 0 as default\n");
				shared_irq = 0;
			}
			if(shared_irq)
			{
				err = devm_request_irq(
					host->dev, gpio_to_irq(gpio), dw_mci_hs_card_detect,
					IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING | IRQF_NO_SUSPEND | IRQF_SHARED,
					DRIVER_NAME, host);
			}
			else
			{
				err = devm_request_irq(
					host->dev, gpio_to_irq(gpio), dw_mci_hs_card_detect,
					IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,
					DRIVER_NAME, host);
			}
			if (err)
				dev_warn(mmc_dev(host->dev),
					 "request gpio irq error\n");
		}

	} else {
		dev_info(host->dev, "cd gpio not available");
	}

	return 0;
}

static int hs_dwmmc_card_busy(struct dw_mci *host)
{
	if ((mci_readl(host, STATUS) & SDMMC_DATA_BUSY) || host->cmd ||
	    host->data || host->mrq || (host->state != STATE_IDLE)) {
		dev_vdbg(host->dev, " card is busy!");
		return 1;
	}

	return 0;
}

#define SCPERCTRL5 (0x214)
#define BIT_SDCARD_IO_SEL18 (0x1 << 2)

static int dw_mci_set_sel18(bool set)
{
	u32 reg;

	reg = readl(sys_base + SCPERCTRL5);
	if (set)
		reg |= BIT_SDCARD_IO_SEL18;
	else
		reg &= ~BIT_SDCARD_IO_SEL18;

	printk(" dw_mci_set_sel18 reg = 0x%x\n", reg);

	writel(reg, sys_base + SCPERCTRL5);

	return 0;
}

static int dw_mci_3_3v_signal_voltage_switch(struct dw_mci_slot *slot)
{
	struct dw_mci *host = slot->host;
	u32 reg;
	int ret = 0;

	ret = dw_mci_set_sel18(0);
	if (ret) {
		dev_err(host->dev, " dw_mci_set_sel18 error!\n");
		return ret;
	}

	/* Wait for 5ms */
	usleep_range(5000, 5500);

	if (host->vqmmc) {
		ret = regulator_set_voltage(host->vqmmc, 2950000, 2950000);
		if (ret) {
			dev_warn(host->dev, "Switching to 3.3V signalling "
					    "voltage failed\n");
			return -EIO;
		}
	} else {
		reg = mci_readl(slot->host, UHS_REG);
		reg &= ~(0x1 << slot->id);
		mci_writel(slot->host, UHS_REG, reg);
	}

	/* Wait for 5ms */
	usleep_range(5000, 5500);

	return ret;
}

static int dw_mci_1_8v_signal_voltage_switch(struct dw_mci_slot *slot)
{
	unsigned long loop_count = 0x100000;
	struct dw_mci *host = slot->host;
	int ret = -1;
	int intrs;

	/* disable interrupt upon voltage switch. handle interrupt here
	 * 	and DO NOT triggle irq */
	mci_writel(host, CTRL,
		   (mci_readl(host, CTRL) & ~SDMMC_CTRL_INT_ENABLE));

	/* stop clock */
	mci_writel(host, CLKENA, (0x0 << 0));
	mci_writel(host, CMD, SDMMC_CMD_ONLY_CLK | SDMMC_VOLT_SWITCH);
	do {
		if (!(mci_readl(host, CMD) & SDMMC_CMD_START))
			break;
		loop_count--;
	} while (loop_count);

	if (!loop_count)
		dev_err(host->dev, " disable clock failed in voltage_switch\n");

	mmiowb();

	if (host->vqmmc) {
		ret = regulator_set_voltage(host->vqmmc, 1800000, 1800000);
		if (ret) {
			dev_warn(host->dev, "Switching to 1.8V signalling "
					    "voltage failed\n");
			return -EIO;
		}
	}

	/* Wait for 5ms */
	usleep_range(10000, 10500);

	ret = dw_mci_set_sel18(1);
	if (ret) {
		dev_err(host->dev, " dw_mci_set_sel18 error!\n");
		return ret;
	}

	/* start clock */
	mci_writel(host, CLKENA, (0x1 << 0));
	mci_writel(host, CMD, SDMMC_CMD_ONLY_CLK | SDMMC_VOLT_SWITCH);
	loop_count = 0x100000;
	do {
		if (!(mci_readl(host, CMD) & SDMMC_CMD_START))
			break;
		loop_count--;
	} while (loop_count);

	if (!loop_count)
		dev_err(host->dev, " enable clock failed in voltage_switch\n");

	/* poll cd interrupt */
	loop_count = 0x100000;
	do {
		intrs = mci_readl(host, RINTSTS);
		if (intrs & SDMMC_INT_CMD_DONE) {
			dev_err(host->dev, " cd 0x%x in voltage_switch\n",
				intrs);
			mci_writel(host, RINTSTS, intrs);
			break;
		}
		loop_count--;
	} while (loop_count);

	if (!loop_count)
		dev_err(host->dev, " poll cd failed in voltage_switch\n");

	/* enable interrupt */
	mci_writel(host, CTRL, (mci_readl(host, CTRL) | SDMMC_CTRL_INT_ENABLE));

	mmiowb();

	return ret;
}

static int dw_mci_priv_voltage_switch(struct mmc_host *mmc, struct mmc_ios *ios)
{
	struct dw_mci_slot *slot = mmc_priv(mmc);
	int ret = 0;

	/* only sd need to switch voltage*/
	if (slot->host->hw_mmc_id != DWMMC_SD_ID)
		return ret;

	pm_runtime_get_sync(mmc_dev(mmc));

	if (ios->signal_voltage == MMC_SIGNAL_VOLTAGE_330)
		ret = dw_mci_3_3v_signal_voltage_switch(slot);
	else if (ios->signal_voltage == MMC_SIGNAL_VOLTAGE_180)
		ret = dw_mci_1_8v_signal_voltage_switch(slot);

	pm_runtime_mark_last_busy(mmc_dev(mmc));
	pm_runtime_put_autosuspend(mmc_dev(mmc));

	return ret;
}

static void dw_mci_set_timeout(struct dw_mci *host)
{
	/* timeout (maximum) */
	mci_writel(host, TMOUT, 0xffffffff);
}

static void dw_mci_hs_tuning_clear_flags(struct dw_mci *host)
{
	host->tuning_sample_flag = 0;
}

static void dw_mci_hs_tuning_set_flags(struct dw_mci *host, int sample, int ok)
{
	if (ok)
		host->tuning_sample_flag |= (1 << sample);
	else
		host->tuning_sample_flag &= ~(1 << sample);
}

/* By tuning, find the best timing condition
 *  1 -- tuning is not finished. And this function should be called again
 *  0 -- Tuning successfully.
 *    If this function be called again, another round of tuning would be start
 *  -1 -- Tuning failed. Maybe slow down the clock and call this function again
 */
static int dw_mci_hs_tuning_find_condition(struct dw_mci *host, int timing)
{
	struct dw_mci_hs_priv_data *priv = host->priv;
	int id = priv->id;
	int sample_min, sample_max;
	int i, j;
	int ret = 0;
	int mask, mask_lenth;
	int d_value = 0;

	if (host->hw_mmc_id == DWMMC_SD_ID) {
		d_value = host->current_div - hs_timing_config[id][timing][1];
		if (timing == MMC_TIMING_SD_HS) {
			sample_max = hs_timing_config[id][timing][4] + d_value;
			sample_min = hs_timing_config[id][timing][5] + d_value;
		} else if ((timing == MMC_TIMING_UHS_SDR50) ||
			   (timing == MMC_TIMING_UHS_SDR104)) {
			sample_max =
			    hs_timing_config[id][timing][4] + 2 * d_value;
			sample_min = hs_timing_config[id][timing][5];
		} else {
			sample_max = hs_timing_config[id][timing][4];
			sample_min = hs_timing_config[id][timing][5];
		}
	} else {
		sample_max = hs_timing_config[id][timing][4];
		sample_min = hs_timing_config[id][timing][5];
	}

	if (sample_max == sample_min) {
		host->tuning_init_sample = (sample_max + sample_min) / 2;
		dw_mci_hs_set_timing(host, id, timing, host->tuning_init_sample,
				     host->current_div);
		dev_info(host->dev,
			 "no need tuning: timing is %d, tuning sample = %d",
			 timing, host->tuning_init_sample);
		return 0;
	}

	if (-1 == host->tuning_current_sample) {

		dw_mci_hs_tuning_clear_flags(host);

		/* set the first sam del as the min_sam_del */
		host->tuning_current_sample = sample_min;
		/* a trick for next "++" */
		host->tuning_current_sample--;
	}

	if (host->tuning_current_sample >= sample_max) {
		/* tuning finish, select the best sam_del */

		/* set sam del to -1, for next tuning */
		host->tuning_current_sample = -1;

		host->tuning_init_sample = -1;
		for (mask_lenth = (((sample_max - sample_min) >> 1) << 1) + 1;
		     mask_lenth >= 1; mask_lenth -= 2) {

			mask = (1 << mask_lenth) - 1;
			for (i = (sample_min + sample_max - mask_lenth + 1) / 2,
			    j = 1;
			     (i <= sample_max - mask_lenth + 1) &&
				 (i >= sample_min);
			     i = ((sample_min + sample_max - mask_lenth + 1) /
				  2) +
				 ((j % 2) ? -1 : 1) * (j / 2)) {
				if ((host->tuning_sample_flag & (mask << i)) ==
				    (mask << i)) {
					host->tuning_init_sample =
					    i + mask_lenth / 2;
					break;
				}

				j++;
			}

			if (host->tuning_init_sample != -1) {
				if ((host->hw_mmc_id == DWMMC_SD_ID) &&
				    (mask_lenth < 3)) {
					dev_info(host->dev,
						 "sd card tuning need slow "
						 "down clk, timing is %d, "
						 "tuning_flag = 0x%x \n",
						 timing,
						 host->tuning_sample_flag);
					return -1;
				} else {
					dev_info(
					    host->dev, "tuning OK: timing is "
						       "%d, tuning sample = "
						       "%d, tuning_flag = 0x%x",
					    timing, host->tuning_init_sample,
					    host->tuning_sample_flag);
					ret = 0;
					break;
				}
			}
		}

		if (-1 == host->tuning_init_sample) {
			host->tuning_init_sample =
			    (sample_min + sample_max) / 2;
			dev_info(host->dev, "tuning err: no good sam_del, "
					    "timing is %d, tuning_flag = 0x%x",
				 timing, host->tuning_sample_flag);
#ifdef CONFIG_HUAWEI_EMMC_DSM
			if (host->hw_mmc_id == DWMMC_EMMC_ID) {
				DSM_EMMC_LOG(host->cur_slot->mmc, DSM_EMMC_TUNING_ERROR,
					"%s:eMMC tuning error: timing is %d, tuning_flag = 0x%x\n", __FUNCTION__, timing, host->tuning_sample_flag);
			}
#endif
			ret = -1;
		}

		dw_mci_hs_set_timing(host, id, timing, host->tuning_init_sample,
				     host->current_div);
		return ret;
	} else {
		host->tuning_current_sample++;
		dw_mci_hs_set_timing(host, id, timing,
				     host->tuning_current_sample,
				     host->current_div);
		return 1;
	}

	return 0;
}

static void dw_mci_hs_tuning_set_current_state(struct dw_mci *host, int ok)
{
	dw_mci_hs_tuning_set_flags(host, host->tuning_current_sample, ok);
}

static int dw_mci_hs_slowdown_clk(struct dw_mci *host, int timing)
{
	struct dw_mci_hs_priv_data *priv = host->priv;
	int id = priv->id;

	host->current_div += 2;

	/* slow down up to half of original freq */
	if (host->current_div > 2 * hs_timing_config[id][timing][1]) {
		host->current_div = 2 * hs_timing_config[id][timing][1];
		return -1;
	} else {
		dev_info(host->dev, "begin slowdown clk, current_div=%d\n",
			 host->current_div);

		dw_mci_hs_set_timing(host, id, timing, host->tuning_init_sample,
				     host->current_div);
	}

	return 0;
}

static int dw_mci_hs_tuning_move(struct dw_mci *host, int timing, int start)
{
	struct dw_mci_hs_priv_data *priv = host->priv;
	int id = priv->id;
	int sample_min, sample_max;
	int loop;

	/* sd card not need move ,just slow down clk */
	if (host->hw_mmc_id == DWMMC_SD_ID)
		return 1;

	sample_max = hs_timing_config[id][timing][4];
	sample_min = hs_timing_config[id][timing][5];
	if (start) {
		host->tuning_move_count = 0;
	}

	for (loop = 0; loop < 2; loop++) {
		host->tuning_move_count++;
		host->tuning_move_sample =
		    host->tuning_init_sample +
		    ((host->tuning_move_count % 2) ? 1 : -1) *
			(host->tuning_move_count / 2);

		if ((host->tuning_move_sample > sample_max) ||
		    (host->tuning_move_sample < sample_min)) {
			continue;
		} else {
			break;
		}
	}

	if ((host->tuning_move_sample > sample_max) ||
	    (host->tuning_move_sample < sample_min)) {
		dw_mci_hs_set_timing(host, id, timing, host->tuning_init_sample,
				     host->current_div);
		dev_info(host->dev, "id = %d, tuning move to init del_sel %d\n",
			 id, host->tuning_init_sample);
		if (dw_mci_hs_slowdown_clk(host, timing) == -1) {
			dev_info(host->dev,
				 "tuning move and slow down clk end \n");
			return 0;
		} else {
			dev_info(host->dev,
				 "slow down clk in tuning move process\n");
			return 1;
		}
	} else {
		dw_mci_hs_set_timing(host, id, timing, host->tuning_move_sample,
				     host->current_div);
		dev_info(host->dev,
			 "id = %d, tuning move to current del_sel %d\n", id,
			 host->tuning_move_sample);
		return 1;
	}
}

#define EMMC_PATTERN_ADDRESS (384*2)
int dw_mci_priv_execute_tuning(struct dw_mci_slot *slot, u32 opcode,
			       struct dw_mci_tuning_data *tuning_data)
{
	struct mmc_host *mmc = slot->mmc;
	struct dw_mci *host = slot->host;
	const struct dw_mci_drv_data *drv_data = host->drv_data;
	unsigned int tuning_loop = MAX_TUNING_LOOP;
	const u8 *tuning_blk_pattern;
	int ret = 0;
	u8 *tuning_blk;
	int blksz;

	int id = host->hw_mmc_id;
	u32 arg = 0;
	unsigned int flags = MMC_RSP_R1 | MMC_CMD_ADTC;

	if (opcode == MMC_SEND_TUNING_BLOCK_HS200) {
		if (mmc->ios.bus_width == MMC_BUS_WIDTH_8) {
			tuning_blk_pattern = tuning_blk_pattern_8bit;
			blksz = 128;
		} else if (mmc->ios.bus_width == MMC_BUS_WIDTH_4) {
			tuning_blk_pattern = tuning_blk_pattern_4bit;
			blksz = 64;
		} else
			return -EINVAL;
	} else if (opcode == MMC_SEND_TUNING_BLOCK) {
		tuning_blk_pattern = tuning_blk_pattern_4bit;
		blksz = 64;
	} else if (opcode == MMC_READ_SINGLE_BLOCK) {
		if (id == 0) /* emmc ddr50*/
			arg = EMMC_PATTERN_ADDRESS;

		blksz = 512;
	} else if (opcode == SD_IO_RW_EXTENDED) {
		arg = 0x200004;
		flags = MMC_RSP_SPI_R5 | MMC_RSP_R5 | MMC_CMD_ADTC;

		blksz = 4;
	} else {
		dev_err(&mmc->class_dev, "Undefined command(%d) for tuning\n",
			opcode);
		return -EINVAL;
	}

	tuning_blk = kmalloc(blksz, GFP_KERNEL);
	if (!tuning_blk)
		return -ENOMEM;

	if ((!drv_data->tuning_find_condition) ||
	    (!drv_data->tuning_set_current_state)) {
		dev_err(&mmc->class_dev, "no tuning find condition method \n");
		goto out;
	}

	pm_runtime_get_sync(mmc_dev(mmc));

	host->flags |= DWMMC_IN_TUNING;
	host->flags &= ~DWMMC_TUNING_DONE;

	do {
		struct mmc_request mrq = { NULL };
		struct mmc_command cmd = { 0 };
		struct mmc_data data = { 0 };
		struct scatterlist sg;

		cmd.opcode = opcode;
		cmd.arg = arg;
		cmd.flags = flags;

		data.blksz = blksz;
		data.blocks = 1;
		data.flags = MMC_DATA_READ;
		data.sg = &sg;
		data.sg_len = 1;

		sg_init_one(&sg, tuning_blk, blksz);
		dw_mci_set_timeout(host);

		mrq.cmd = &cmd;
		mrq.stop = NULL;
		mrq.data = &data;


		ret = drv_data->tuning_find_condition(host, mmc->ios.timing);
		if (ret == -1) {
			if ((host->hw_mmc_id == DWMMC_SD_ID) &&
			    (drv_data->slowdown_clk)) {
				ret = drv_data->slowdown_clk(host,
							     mmc->ios.timing);
				if (ret)
					break;
			}
		} else if (0 == ret)
			break;

		mmc_wait_for_req(mmc, &mrq);

		if (!cmd.error && !data.error) {
			drv_data->tuning_set_current_state(host, 1);
		} else {
			drv_data->tuning_set_current_state(host, 0);
			dev_dbg(&mmc->class_dev,
				"Tuning error: cmd.error:%d, data.error:%d\n",
				cmd.error, data.error);
		}

	} while (tuning_loop--);

	host->flags &= ~DWMMC_IN_TUNING;
	if (!ret) {
		host->flags |= DWMMC_TUNING_DONE;
	}

	host->tuning_move_start = 1;
out:
	kfree(tuning_blk);

	pm_runtime_mark_last_busy(mmc_dev(mmc));
	pm_runtime_put_autosuspend(mmc_dev(mmc));

	return 0;
}

/* Common capabilities of hi3650 SoC */
static unsigned long hs_dwmmc_caps[3] = {
	/* emmc */
	MMC_CAP_NONREMOVABLE | MMC_CAP_8_BIT_DATA | MMC_CAP_MMC_HIGHSPEED |
	    MMC_CAP_ERASE | MMC_CAP_CMD23,
	/* sd */
	MMC_CAP_DRIVER_TYPE_A | MMC_CAP_4_BIT_DATA | MMC_CAP_SD_HIGHSPEED |
	    MMC_CAP_MMC_HIGHSPEED | MMC_CAP_UHS_SDR12 | MMC_CAP_UHS_SDR25 |
	    MMC_CAP_UHS_SDR50 | MMC_CAP_UHS_SDR104,
	/* sdio */
	MMC_CAP_4_BIT_DATA | MMC_CAP_SDIO_IRQ | MMC_CAP_SD_HIGHSPEED | MMC_CAP_NONREMOVABLE,
};

static const struct dw_mci_drv_data hs_drv_data = {
	.caps                        = hs_dwmmc_caps,
	.init                        = dw_mci_hs_priv_init,
	.set_ios                     = dw_mci_hs_set_ios,
	.setup_clock                 = dw_mci_hs_setup_clock,
	.prepare_command             = dw_mci_hs_prepare_command,
	.parse_dt                    = dw_mci_hs_parse_dt,
	.cd_detect_init              = dw_mci_hs_cd_detect_init,
	.tuning_find_condition       = dw_mci_hs_tuning_find_condition,
	.tuning_set_current_state    = dw_mci_hs_tuning_set_current_state,
	.tuning_move                 = dw_mci_hs_tuning_move,
	.slowdown_clk                = dw_mci_hs_slowdown_clk,
	.execute_tuning              = dw_mci_priv_execute_tuning,
	.start_signal_voltage_switch = dw_mci_priv_voltage_switch,
};

static const struct of_device_id dw_mci_hs_match[] = {
	{
		.compatible 	= "hisilicon,hi3635-dw-mshc",
		.data 		= &hs_drv_data,
	},
	{},
};

MODULE_DEVICE_TABLE(of, dw_mci_hs_match);

int dw_mci_hs_probe(struct platform_device *pdev)
{
	const struct dw_mci_drv_data *drv_data = NULL;
	const struct of_device_id *match = NULL;
	int err;

	match = of_match_node(dw_mci_hs_match, pdev->dev.of_node);
	if(!match)
		return -1;
	drv_data = match->data;

	err = dw_mci_hs_get_resource();
	if (err)
		return err;

	err = dw_mci_pltfm_register(pdev, drv_data);
	if(err)
		return err;

	pm_runtime_set_active(&pdev->dev);
	pm_runtime_enable(&pdev->dev);
	pm_runtime_set_autosuspend_delay(&pdev->dev, 50);
	pm_runtime_use_autosuspend(&pdev->dev);
	pm_suspend_ignore_children(&pdev->dev, 1);
#ifdef  CONFIG_HUAWEI_DSM
	if(!dclient)
	    dclient = dsm_register_client(&dsm_dw_mmc);
#endif
	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int dw_mci_hs_suspend(struct device *dev)
{
	int ret;
	struct dw_mci *host = dev_get_drvdata(dev);
	struct dw_mci_hs_priv_data *priv = host->priv;
	dev_info(host->dev, " %s ++ \n", __func__);

	pm_runtime_get_sync(dev);

	if (priv->gpio_cd) {
			disable_irq_nosync(gpio_to_irq(priv->gpio_cd));
			dev_info(host->dev, " disable gpio detect \n");
		}

	ret = dw_mci_suspend(host);
	if (ret)
		return ret;
	priv->old_timing = -1;
	priv->old_power_mode = MMC_POWER_OFF;
	if (!IS_ERR(host->biu_clk))
		clk_disable_unprepare(host->biu_clk);

	if (!IS_ERR(host->ciu_clk))
		clk_disable_unprepare(host->ciu_clk);

	dw_mci_hs_set_controller(host, 1);

	host->current_speed = 0;

	pm_runtime_mark_last_busy(dev);
	pm_runtime_put_autosuspend(dev);

	dev_info(host->dev, " %s -- \n", __func__);
	return 0;
}

static int dw_mci_hs_resume(struct device *dev)
{
	int ret;
	struct dw_mci *host = dev_get_drvdata(dev);
	struct dw_mci_hs_priv_data *priv = host->priv;
	dev_info(host->dev, " %s ++ \n", __func__);

	pm_runtime_get_sync(dev);
	dw_mci_hs_set_controller(host, 0);

	if (!IS_ERR(host->biu_clk)) {
			if (clk_prepare_enable(host->biu_clk))
				dev_err(host->dev, "biu_clk clk_prepare_enable failed\n");
		}

	if (!IS_ERR(host->ciu_clk)) {
			if (clk_prepare_enable(host->ciu_clk))
				dev_err(host->dev, "ciu_clk clk_prepare_enable failed\n");
		}

	host->flags &= ~DWMMC_IN_TUNING;
	host->flags &= ~DWMMC_TUNING_DONE;

	/* restore controller specified setting */
	dw_mci_hs_priv_setting(host);
	ret = dw_mci_resume(host);
	if (ret)
		return ret;
	if (priv->gpio_cd) {
			enable_irq(gpio_to_irq(priv->gpio_cd));
			dev_info(host->dev, " enable gpio detect \n");
		}

	pm_runtime_mark_last_busy(dev);
	pm_runtime_put_autosuspend(dev);

	dev_info(host->dev, " %s -- \n", __func__);
	return 0;
}
#endif

#ifdef CONFIG_PM_RUNTIME
static int dw_mci_hs_runtime_suspend(struct device *dev)
{
	struct dw_mci *host = dev_get_drvdata(dev);
	dev_vdbg(host->dev, " %s ++ \n", __func__);

	if (hs_dwmmc_card_busy(host))
		return 0;

	if (!IS_ERR(host->biu_clk))
		clk_disable_unprepare(host->biu_clk);

	if (!IS_ERR(host->ciu_clk))
		clk_disable_unprepare(host->ciu_clk);

	dev_vdbg(host->dev, " %s -- \n", __func__);

	return 0;
}

static int dw_mci_hs_runtime_resume(struct device *dev)
{
	struct dw_mci *host = dev_get_drvdata(dev);
	dev_vdbg(host->dev, " %s ++ \n", __func__);

	if (!IS_ERR(host->biu_clk)) {
			if (clk_prepare_enable(host->biu_clk))
				dev_err(host->dev, "biu_clk clk_prepare_enable failed\n");
		}

	if (!IS_ERR(host->ciu_clk)) {
			if (clk_prepare_enable(host->ciu_clk))
				dev_err(host->dev, "ciu_clk clk_prepare_enable failed\n");
		}

	dev_vdbg(host->dev, " %s -- \n", __func__);

	return 0;
}
#endif

static const struct dev_pm_ops dw_mci_hs_pmops = {
	SET_SYSTEM_SLEEP_PM_OPS(dw_mci_hs_suspend, dw_mci_hs_resume)
	SET_RUNTIME_PM_OPS(dw_mci_hs_runtime_suspend,
					dw_mci_hs_runtime_resume, NULL)
};

static struct platform_driver dw_mci_hs_pltfm_driver = {
	.probe		= dw_mci_hs_probe,
	.remove		= dw_mci_pltfm_remove,
	.driver		= {
		.name		= DRIVER_NAME,
		.of_match_table	= of_match_ptr(dw_mci_hs_match),
		.pm 		= &dw_mci_hs_pmops,
	},
};

module_platform_driver(dw_mci_hs_pltfm_driver);

MODULE_DESCRIPTION("Hisilicon Specific DW-MSHC Driver Extension");
MODULE_LICENSE("GPL v2");
