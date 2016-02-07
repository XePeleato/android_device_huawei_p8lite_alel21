/* Copyright (c) 2008-2011, Hisilicon Tech. Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *	 * Redistributions of source code must retain the above copyright
 *	   notice, this list of conditions and the following disclaimer.
 *	 * Redistributions in binary form must reproduce the above
 *	   copyright notice, this list of conditions and the following
 *	   disclaimer in the documentation and/or other materials provided
 *	   with the distribution.
 *	 * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *	   contributors may be used to endorse or promote products derived
 *	   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef __BALONG_FB_PANEL_H__
#define __BALONG_FB_PANEL_H__

#include <linux/regulator/consumer.h>
#include <asm/io.h>
#include "balong_fb_def.h"
#include "balong_ade.h"

/* panel type list */
#define PANEL_NO			0xffff	/* No Panel */
#define PANEL_LDI			1	/* internal LCDC type */
#define PANEL_HDMI		2	/* HDMI TV */
#define PANEL_MIPI_VIDEO	3	/* MIPI */
#define PANEL_MIPI_CMD	4	/* MIPI */
#define PANEL_DP			5	/* Display Port */
#define PANEL_MIPI2RGB	6	/* MIPI to RGB */

/* device name */
#define DEV_NAME_LDI				"ldi"
#define DEV_NAME_HDMI			    "hdmi"
#define DEV_NAME_DP				    "displayport"
#define DEV_NAME_MIPI2RGB		    "mipi2rgb"
#define DEV_NAME_MIPIDSI			"mipi_dsi"
#define DEV_NAME_FB				    "balong_fb"
#define DEV_NAME_LCD_BKL		    "lcd_backlight0"

#define VCC_BACKLIGHT_NAME		"lcd_backlight"
#define VCC_LCDBIAS_NAME		"vcc_lcdbias"
#define VCC_LCD_VSN_NAME		"lcd_vsn"
#define VCC_LCD_VSP_NAME		"lcd_vsp"

#define LCD_INVALID_GPIO_NUM    0xffff

/************ BEGIN *****************/
/* LCD init step */
enum {
	LCD_INIT_POWER_ON = 0,
	LCD_INIT_SEND_SEQUENCE
};

/* dtype for vcc */
enum {
	DTYPE_VCC_GET,
	DTYPE_VCC_PUT,
	DTYPE_VCC_ENABLE,
	DTYPE_VCC_DISABLE,
	DTYPE_VCC_SET_VOLTAGE,
	DTYPE_VCC_SET_CURRENT,
};

/* vcc desc */
struct vcc_desc {
	int dtype;
	char *id;
	struct regulator **regulator;
	int waittype;
	int wait;
	int min_uV;
	int max_uV;
    int min_uA;
	int max_uA;
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
    DTYPE_GPIO_GET,
	DTYPE_GPIO_REQUEST,
	DTYPE_GPIO_FREE,
	DTYPE_GPIO_INPUT,
	DTYPE_GPIO_OUTPUT,
};

/* gpio desc */
struct gpio_desc {
	int dtype;
	int waittype;
	int wait;
	char *label;
	u32 *gpio;
	int value;
};

enum {
    WAIT_TYPE_US = 0,
    WAIT_TYPE_MS
};

/********** END **************/

typedef struct panel_id_s {
	u32 id;
	u32 type;
} panel_id_type;

struct ldi_panel_info {
	u32 h_back_porch;
	u32 h_front_porch;
	u32 h_pulse_width;
	u32 v_back_porch;
	u32 v_front_porch;
	u32 v_pulse_width;

	u8 hsync_plr;
	u8 vsync_plr;
	u8 pixelclk_plr;
	u8 data_en_plr;
	u8 disp_mode;
	u8 reservd[3];
};

struct lcd_pinctrl_info {
    struct pinctrl_state *pinctrl_state_normal;
    struct pinctrl_state *pinctrl_state_idle;
    struct pinctrl       *pinctrl;
};

/* DSI PHY configuration */

struct mipi_dsi_phy_register {
	u32 clk_t_lpx;
	u32 clk_t_hs_prepare;
	u32 clk_t_hs_zero;
	u32 clk_t_hs_trial;
	u32 clk_t_wakeup;
	u32 data_t_lpx;
	u32 data_t_hs_prepare;
	u32 data_t_hs_zero;
	u32 data_t_hs_trial;
	u32 data_t_ta_go;
	u32 data_t_ta_get;
	u32 data_t_wakeup;
	u32 rg_hstx_ckg_sel;
	u32 rg_pll_fbd_div5f;
	u32 rg_pll_fbd_div1f;
	u32 rg_pll_fbd_2p;
	u32 rg_pll_enbwt;
	u32 rg_pll_fbd_p;
	u32 rg_pll_fbd_s;
	u32 rg_pll_pre_div1p;
	u32 rg_pll_pre_p;
	u32 rg_pll_vco_750M;
	u32 rg_pll_lpf_rs;
	u32 rg_pll_lpf_cs;
	u32 phy_clklp2hs_time;
	u32 phy_clkhs2lp_time;
	u32 phy_lp2hs_time;
	u32 phy_hs2lp_time;
	u32 clk_to_data_delay;
	u32 data_to_clk_delay;
	u32 lane_byte_clk;
	u32 clk_division;
	u32 burst_mode;
};

struct mipi_dsi_phy_ctrl {
	u32 burst_mode;
	u32 lane_byte_clk;
	u32 clk_division;
	u32 lp2hs_time;
	u32 hs2lp_time;
	u32 clk_lp2hs_time;
	u32 clk_hs2lp_time;
	u32 hsfreqrange;
	u32 pll_unlocking_filter;
	u32 cp_current;
	u32 lpf_ctrl;
	u32 n_pll;
	u32 m_pll_1;
	u32 m_pll_2;
	u32 factors_effective;
};

struct dsi_phy_seq_info {
    u32 min_range;
    u32 max_range;
    u32 rg_pll_vco_750M;
    u32 rg_hstx_ckg_sel;
};


struct mipi_panel_info {
	u8 vc;
	u8 lane_nums;
	u8 color_mode;
	u8 reserved;
	u32 dphy_freq; /* dphy_freq*/
	/*u32 dsi_pclk_rate;*/
};

struct sbl_panel_info{
	u32 bl_max;
	u32 cal_a;
	u32 cal_b;
	u32 str_limit;
};

struct balong_panel_info {
	u32 type;
	u32 xres;
	u32 yres;
	u32 width;
	u32 height;
	u32 clk_rate;
	u8 bpp;
	u8 orientation;
	u8 s3d_frm;
	u8 bgr_fmt;
	u8 frame_rate;
	u8 bl_set_type;

	volatile bool display_on;
	u8 lcd_init_step;

	u8 sbl_enable;
	u8 frc_enable;
	u8 esd_enable;
	u8 reserved[5];
	struct ldi_panel_info ldi;
	struct mipi_panel_info mipi;
	struct sbl_panel_info sbl;
	struct lcd_pinctrl_info pinctrl;
    struct lcd_dirty_region_info dirty_region_info;

	u32 gpio_reset;
	u32 gpio_power;
	u32 gpio_lcd_id0;
	u32 gpio_lcd_id1;
	u32 gpio_drv_en;
	u32 gpio_lcdmipi_power;

    /* scharg regulator */
    struct regulator *lcd_bl_vcc;
    struct regulator *lcd_bias_vcc;
    struct regulator *lcd_vsn_vcc;
    struct regulator *lcd_vsp_vcc;


	u32 bl_max;
	u32 bl_min;
	u32 gpio_pwm0;
	u32 gpio_pwm1;
	u8* pwm_base;
	struct clk *pwm_clk;
	struct iomux_block *pwm_block;
	struct block_config *pwm_block_config;
};

struct balong_fb_data_type;

struct balong_fb_panel_data {
	struct balong_panel_info *panel_info;

	/* function entry chain */
	int (*on) (struct platform_device *pdev);
	int (*off) (struct platform_device *pdev);
	int (*remove) (struct platform_device *pdev);
	int (*set_backlight) (struct platform_device *pdev);
	int (*set_timing) (struct platform_device *pdev);
	int (*set_fastboot) (struct platform_device *pdev);
	int (*set_frc)(struct platform_device *pdev, int target_fps);
	int (*set_cabc)(struct platform_device *pdev, int value);
	//add for SBL (SRE on LCD IC)
	int (*set_sre)(struct platform_device *pdev, int enable, int value);
	int (*check_esd) (struct platform_device *pdev);
#if PARTIAL_UPDATE
	int (*set_display_region)(struct platform_device *pdev, int x, int y, int width, int height);
#endif
#if LCD_CHECK_MIPI_SUPPORT
    int (*check_mipi_tr)(struct platform_device *pdev);
    int (*set_inversion_type)(struct platform_device *pdev, unsigned int inversion_mode);
#endif

    int (*lcd_cabc_mode_store) (struct platform_device *pdev, int cabc);
    int (*lcd_gram_check_show) (struct platform_device *pdev);
    int (*lcd_inversion_mode_store) (struct platform_device *pdev, unsigned int inversion_mode);
    int (*lcd_inversion_mode_show) (struct platform_device *pdev);

	struct platform_device *next;
};


/*******************************************************************************
** FUNCTIONS PROTOTYPES
*/

void set_reg(u8* addr, u32 val, u8 bw, u8 bs);
void set_value(u32* addr, u32 val, u8 bw, u8 bs);

struct platform_device *balong_fb_device_alloc(struct balong_fb_panel_data *pdata,
	u32 type, u32 index, u32 *graphic_ch);
void balong_fb_device_free(struct platform_device *pdev);

int panel_next_on(struct platform_device *pdev);
int panel_next_off(struct platform_device *pdev);
int panel_next_remove(struct platform_device *pdev);
int panel_next_set_backlight(struct platform_device *pdev);
int panel_next_set_timing(struct platform_device *pdev);
int panel_next_set_frc(struct platform_device *pdev, int target_fps);
int panel_next_check_esd(struct platform_device *pdev);
#if LCD_CHECK_MIPI_SUPPORT
int panel_next_check_mipi_tr(struct platform_device *pdev);
int panel_next_set_inversion_type(struct platform_device *pdev, unsigned int inversion_mode);
#endif
int mipi_dsi_read_reg(struct balong_fb_data_type *balongfd, u32 reg, u32 *val, u32 rd_type);
int pwm_set_backlight(int bl_lvl, struct balong_panel_info *pinfo);
int balong_fb_set_clock_rate(struct balong_fb_data_type *balongfd, struct clk *clk, u32 clk_rate);

/******************************************************************************/

#define CLK_PWM0_NAME	        "clk_pwm0"

#define REG_BASE_PWM0_NAME	"reg_base_pwm0"
#define REG_BASE_ADE_NAME	"reg_base_ade"
#define REG_BASE_DSI0_NAME	"reg_base_dsi0"



#define GPIO_LCD_POWER_NAME	"gpio_lcd_power"
#define GPIO_LCD_PWM0_NAME	"gpio_pwm0"            /* V8R1/V8R2 not use */
#define GPIO_LCD_PWM1_NAME	"gpio_pwm1"            /* V8R1/V8R2 not use */
#define GPIO_LCD_ID0_NAME	"gpio_lcd_id0"         /* V8R1/V8R2 not use */
#define GPIO_LCD_ID1_NAME	"gpio_lcd_id1"         /* V8R1/V8R2 not use */
#define GPIO_TP_RESET_NAME	"gpio_tp_reset"        /* V8R1/V8R2 not use */
#define GPIO_LCD_DRV_NAME   "gpio_lcd_drv"         /* V8R1/V8R2 not use */
#define GPIO_LCD_DRVEN_NAME  "gpio_lcd_drv_en"
#define GPIO_LCDMIPI_POWER_NAME	"gpio_lcdmipi_power"


#define IOMUX_LCD_NAME	"block_lcd"
#define IOMUX_PWM_NAME	"block_pwm"

#define DEFAULT_PWM_CLK_RATE	(13 * 1000000)

/*  not used in V8R2  */
int PWM_RESOUTCE(struct platform_device *pdev, struct balong_panel_info *pinfo, struct resource *res);

int PWM_CLK_GET(struct balong_panel_info *pinfo);
void PWM_CLK_PUT(struct balong_panel_info *pinfo);
void PWM_GPIO_REQUEST(struct balong_panel_info *pinfo);
void PWM_GPIO_FREE(struct balong_panel_info *pinfo);
/********************/

extern int vcc_cmds_tx(struct platform_device *pdev, struct vcc_desc *cmds, int cnt);
extern int pinctrl_cmds_tx(struct platform_device *pdev, struct pinctrl_cmd_desc *cmds, int cnt);
extern int gpio_cmds_tx(struct platform_device *pdev, struct gpio_desc *cmds, int cnt);
extern int get_resource_from_dts(struct platform_device *pdev, struct balong_panel_info *pinfo);


#endif /* __BALONG_FB_PANEL_H__ */
