/* Copyright (c) 2013-2014, Hisilicon Tech. Co., Ltd. All rights reserved.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 and
* only version 2 as published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
* GNU General Public License for more details.
*
*/
#ifndef HISI_FB_PANEL_H
#define HISI_FB_PANEL_H

#include "hisi_fb_def.h"
#include "hisi_mipi_dsi.h"
#if defined(CONFIG_ARCH_HI3630FPGA) || defined(CONFIG_HISI_3635_FPGA) \
	|| defined(CONFIG_HISI_3650_FPGA)
#include "hisi_rgb2mipi.h"
#endif


/* panel type list */
#define PANEL_NO	0xffff	/* No Panel */
#define PANEL_LCDC	BIT(1)	/* internal LCDC type */
#define PANEL_HDMI	BIT(2)	/* HDMI TV */
#define PANEL_MIPI_VIDEO	BIT(3)	/* MIPI */
#define PANEL_MIPI_CMD	BIT(4)	/* MIPI */
#define PANEL_DUAL_MIPI_VIDEO	BIT(5)	/* DUAL MIPI */
#define PANEL_DUAL_MIPI_CMD	BIT(6)	/* DUAL MIPI */
#define PANEL_EDP	BIT(7)	/* LVDS */
#define PANEL_MIPI2RGB	BIT(8)	/* MIPI to RGB */
#define PANEL_RGB2MIPI	BIT(9)	/* RGB to MIPI */
#define PANEL_OFFLINECOMPOSER	BIT(10)	/* offline composer */
#define PANEL_WRITEBACK	BIT(11)	/* Wifi display */

/* dts initial */
#define DTS_FB_RESOURCE_INIT_READY	BIT(0)
#define DTS_PWM_READY	BIT(1)
//#define DTS_BLPWM_READY	BIT(2)
#define DTS_SPI_READY	BIT(3)
#define DTS_PANEL_PRIMARY_READY	BIT(4)
#define DTS_PANEL_EXTERNAL_READY	BIT(5)
#define DTS_PANEL_OFFLINECOMPOSER_READY	BIT(6)
#define DTS_PANEL_WRITEBACK_READY	BIT(7)

/* device name */
#define DEV_NAME_DSS_DPE		"dss_dpe"
#define DEV_NAME_SPI				"spi_dev0"
#define DEV_NAME_HDMI			"hdmi"
#define DEV_NAME_EDP				"edp"
#define DEV_NAME_MIPI2RGB		"mipi2rgb"
#define DEV_NAME_RGB2MIPI		"rgb2mipi"
#define DEV_NAME_MIPIDSI			"mipi_dsi"
#define DEV_NAME_FB				"hisi_fb"
#define DEV_NAME_PWM			"hisi_pwm"
#define DEV_NAME_BLPWM			"hisi_blpwm"
#define DEV_NAME_LCD_BKL		"lcd_backlight0"

/* vcc name */
#define REGULATOR_PDP_NAME	"regulator_dsssubsys"
#define REGULATOR_SDP_NAME	"regulator_sdp"
#define REGULATOR_ADP_NAME	"regulator_adp"

/* irq name */
#define IRQ_PDP_NAME	"irq_pdp"
#define IRQ_SDP_NAME	"irq_sdp"
#define IRQ_ADP_NAME	"irq_adp"
#define IRQ_MCU_PDP_NAME	"irq_mcu_pdp"
#define IRQ_MCU_SDP_NAME	"irq_mcu_sdp"
#define IRQ_MCU_ADP_NAME	"irq_mcu_adp"
#define IRQ_DSI0_NAME	"irq_dsi0"
#define IRQ_DSI1_NAME	"irq_dsi1"

/* dts compatible */
#define DTS_COMP_FB_NAME	"hisilicon,hisifb"
#define DTS_COMP_PWM_NAME	"hisilicon,hisipwm"
#define DTS_COMP_BLPWM_NAME	"hisilicon,hisiblpwm"

/* backlight type */
#define BL_SET_BY_NONE	BIT(0)
#define BL_SET_BY_PWM	BIT(1)
#define BL_SET_BY_BLPWM	BIT(2)
#define BL_SET_BY_MIPI	BIT(3)

/* LCD init step */
enum {
	LCD_INIT_NONE = 0,
	LCD_INIT_POWER_ON,
	LCD_INIT_LDI_SEND_SEQUENCE,
	LCD_INIT_MIPI_LP_SEND_SEQUENCE,
	LCD_INIT_MIPI_HS_SEND_SEQUENCE,
};

enum IFBC_TYPE {
	IFBC_TYPE_NON = 0x0, /* no compression*/
	IFBC_TYPE_HISI_0 = 0x1, /* 1/2 compression ratio,  non-sorting */
	IFBC_TYPE_HISI_1 = 0x9, /* 1/3 compression ratio,  non-sorting */
	IFBC_TYPE_HISI_2 = 0x11, /* 1/2 compression ratio,  sorting*/
	IFBC_TYPE_HISI_3 = 0x19, /* 1/3 compression ratio,  sorting */
	IFBC_TYPE_ORISE = 0x2,
	IFBC_TYPE_RSP_0 = 0x3, /* RSP 1/2 compression ratio*/
	IFBC_TYPE_RSP_1 = 0x23, /* RSP 1/3 compression ratio*/
	IFBC_TYPE_NOVATEK = 0x4,
	IFBC_TYPE_HIMAX = 0x5,

};

enum VSYNC_CTRL_TYPE {
	VSYNC_CTRL_NONE = 0x0,
	VSYNC_CTRL_ISR_OFF = 0x1,
	VSYNC_CTRL_MIPI_ULPS = 0x2,
	VSYNC_CTRL_CLK_OFF = 0x4,
	VSYNC_CTRL_VCC_OFF = 0x8,
};

/* resource desc */
struct resource_desc {
	uint32_t flag;
	char *name;
	uint32_t *value;
};

/* dtype for vcc */
enum {
	DTYPE_VCC_GET,
	DTYPE_VCC_PUT,
	DTYPE_VCC_ENABLE,
	DTYPE_VCC_DISABLE,
	DTYPE_VCC_SET_VOLTAGE,
};

/* vcc desc */
struct vcc_desc {
	int dtype;
	char *id;
	struct regulator **regulator;
	int min_uV;
	int max_uV;
};

/* pinctrl operation */
enum {
	DTYPE_PINCTRL_GET,
	DTYPE_PINCTRL_STATE_GET,
	DTYPE_PINCTRL_SET,
	DTYPE_PINCTRL_PUT,
};

/* pinctrl state */
enum {
	DTYPE_PINCTRL_STATE_DEFAULT,
	DTYPE_PINCTRL_STATE_IDLE,
};

/* pinctrl data */
struct pinctrl_data {
	struct pinctrl *p;
	struct pinctrl_state *pinctrl_def;
	struct pinctrl_state *pinctrl_idle;
};
struct pinctrl_cmd_desc {
	int dtype;
	struct pinctrl_data *pctrl_data;
	int mode;
};

/* dtype for gpio */
enum {
	DTYPE_GPIO_REQUEST,
	DTYPE_GPIO_FREE,
	DTYPE_GPIO_INPUT,
	DTYPE_GPIO_OUTPUT,
};

enum {
	LCD_CHECK_REG_FAIL = -1,
	LCD_CHECK_REG_PASS = 0,
};

enum {
	LCD_MIPI_DETECT_FAIL = -1,
	LCD_MIPI_DETECT_PASS = 0,
};

/* gpio desc */
struct gpio_desc {
	int dtype;
	int waittype;
	int wait;
	char *label;
	uint32_t *gpio;
	int value;
};

struct spi_cmd_desc {
	int reg_len;
	char *reg;
	int val_len;
	char *val;
	int waittype;
	int wait;
};

struct ldi_panel_info {
	uint32_t h_back_porch;
	uint32_t h_front_porch;
	uint32_t h_pulse_width;

	/*
	** note: vbp > 8 if used overlay compose,
	** also lcd vbp > 8 in lcd power on sequence
	*/
	uint32_t v_back_porch;
	uint32_t v_front_porch;
	uint32_t v_pulse_width;

	uint8_t hsync_plr;
	uint8_t vsync_plr;
	uint8_t pixelclk_plr;
	uint8_t data_en_plr;

	/* for cabc */
	uint8_t dpi0_overlap_size;
	uint8_t dpi1_overlap_size;
};

/* DSI PHY configuration */
struct mipi_dsi_phy_ctrl {
	uint32_t burst_mode;
	uint32_t lane_byte_clk;
	uint32_t clk_division;
	uint32_t clk_lane_lp2hs_time;
	uint32_t clk_lane_hs2lp_time;
	uint32_t data_lane_lp2hs_time;
	uint32_t data_lane_hs2lp_time;
	uint32_t hsfreqrange;
	uint32_t pll_unlocking_filter;
	uint32_t cp_current;
	uint32_t lpf_ctrl;
	uint32_t n_pll;
	uint32_t m_pll_1;
	uint32_t m_pll_2;
	uint32_t factors_effective;
};

struct mipi_panel_info {
	uint8_t vc;
	uint8_t lane_nums;
	uint8_t color_mode;
	uint32_t dsi_bit_clk; /* clock lane(p/n) */
	/*uint32_t dsi_pclk_rate;*/
};

struct sbl_panel_info {
	uint32_t strength_limit;
	uint32_t calibration_a;
	uint32_t calibration_b;
	uint32_t calibration_c;
	uint32_t calibration_d;
	uint32_t t_filter_control;
	uint32_t backlight_min;
	uint32_t backlight_max;
	uint32_t backlight_scale;
	uint32_t ambient_light_min;
	uint32_t filter_a;
	uint32_t filter_b;
	uint32_t logo_left;
	uint32_t logo_top;
};

struct hisi_panel_info {
	uint32_t type;
	uint32_t xres;
	uint32_t yres;
	uint32_t width;
	uint32_t height;
	uint8_t bpp;
	uint8_t orientation;
	uint8_t bgr_fmt;
	uint8_t bl_set_type;
	uint32_t bl_min;
	uint32_t bl_max;
	uint64_t pxl_clk_rate;
	uint8_t ifbc_type;
	uint8_t vsync_ctrl_type;

	uint8_t lcd_init_step;

	uint8_t sbl_support;
	uint8_t frc_enable;
	uint8_t esd_enable;
	uint8_t dirty_region_updt_support;

	struct spi_device *spi_dev;
	struct ldi_panel_info ldi;
	struct mipi_panel_info mipi;
	struct sbl_panel_info smart_bl;
};

struct hisi_fb_data_type;
struct dss_rect;

struct hisi_fb_panel_data {
	struct hisi_panel_info *panel_info;

	/* function entry chain */
	int (*set_fastboot) (struct platform_device *pdev);
	int (*on) (struct platform_device *pdev);
	int (*off) (struct platform_device *pdev);
	int (*remove) (struct platform_device *pdev);
	int (*set_backlight) (struct platform_device *pdev);
	int (*sbl_ctrl) (struct platform_device *pdev, int enable);
	int (*vsync_ctrl) (struct platform_device *pdev, int enable);

	int (*frc_handle)(struct platform_device *pdev, int fps);
	int (*esd_handle) (struct platform_device *pdev);
	int (*set_display_region)(struct platform_device *pdev, struct dss_rect *dirty);
	int (*set_pixclk_rate)(struct platform_device *pdev);
	int (*lcd_check_reg)(struct platform_device *pdev);
	int (*lcd_mipi_detect)(struct platform_device *pdev);
	int (*set_disp_resolution)(struct platform_device *pdev);

	struct platform_device *next;
};


/*******************************************************************************
** FUNCTIONS PROTOTYPES
*/
extern uint32_t g_dts_resouce_ready;

int resource_cmds_tx(struct platform_device *pdev,
	struct resource_desc *cmds, int cnt);
int vcc_cmds_tx(struct platform_device *pdev, struct vcc_desc *cmds, int cnt);
int pinctrl_cmds_tx(struct platform_device *pdev, struct pinctrl_cmd_desc *cmds, int cnt);
int gpio_cmds_tx(struct gpio_desc *cmds, int cnt);
#if defined(CONFIG_ARCH_HI3630FPGA) || defined(CONFIG_HISI_3635_FPGA) \
	|| defined(CONFIG_HISI_3650_FPGA)
extern struct spi_device *g_spi_dev;
int spi_cmds_tx(struct spi_device *spi, struct spi_cmd_desc *cmds, int cnt);
#endif

int panel_next_set_fastboot(struct platform_device *pdev);
int panel_next_on(struct platform_device *pdev);
int panel_next_off(struct platform_device *pdev);
int panel_next_remove(struct platform_device *pdev);
int panel_next_set_backlight(struct platform_device *pdev);
int panel_next_sbl_ctrl(struct platform_device *pdev, int enable);
int panel_next_vsync_ctrl(struct platform_device *pdev, int enable);
int panel_next_frc_handle(struct platform_device *pdev, int fps);
int panel_next_esd_handle(struct platform_device *pdev);
int panel_next_set_display_region(struct platform_device *pdev, struct dss_rect *dirty);
int panel_next_lcd_check_reg(struct platform_device *pdev);
int panel_next_lcd_mipi_detect(struct platform_device *pdev);

int hisi_pwm_set_backlight(struct hisi_fb_data_type *hisifd);
int hisi_pwm_off(struct platform_device *pdev);
int hisi_pwm_on(struct platform_device *pdev);

int hisi_blpwm_set_backlight(struct hisi_fb_data_type *hisifd);
int hisi_blpwm_off(struct platform_device *pdev);
int hisi_blpwm_on(struct platform_device *pdev);

bool is_ldi_panel(struct hisi_fb_data_type *hisifd);
bool is_mipi_cmd_panel(struct hisi_fb_data_type *hisifd);
bool is_mipi_video_panel(struct hisi_fb_data_type *hisifd);
bool is_mipi_panel(struct hisi_fb_data_type *hisifd);
bool is_dual_mipi_panel(struct hisi_fb_data_type *hisifd);
bool is_ifbc_panel(struct hisi_fb_data_type *hisifd);
bool is_hisi_writeback_panel(struct hisi_fb_data_type *hisifd);

void hisi_fb_device_set_status0(uint32_t status);
int hisi_fb_device_set_status1(struct hisi_fb_data_type *hisifd);
bool hisi_fb_device_probe_defer(uint32_t panel_type);


#endif /* HISI_FB_PANEL_H */
