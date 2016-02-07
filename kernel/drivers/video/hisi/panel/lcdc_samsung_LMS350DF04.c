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

#include "hisi_fb.h"


#define DTS_COMP_LCDC_SAMSUNG_LMS350DF04	"hisilicon,lcdc_samsung_LMS350DF04"
static struct hisi_fb_panel_data samsung_panel_data;


#define SPI_REG_TYPE	0x74
#define SPI_VAL_TYPE	0x76


/*******************************************************************************
** Power ON/OFF Sequence
*/

static char on_reg1[] = { SPI_REG_TYPE, 0x00, 0x07, };
static char on_val1[] = { SPI_VAL_TYPE, 0x00, 0x00, };

static char on_reg2[] = { SPI_REG_TYPE, 0x00, 0x11, };
static char on_val2[] = { SPI_VAL_TYPE, 0x22, 0x2F, };

static char on_reg3[] = { SPI_REG_TYPE, 0x00, 0x12, };
static char on_val3[] = { SPI_VAL_TYPE, 0x0F, 0x00, };

static char on_reg4[] = { SPI_REG_TYPE, 0x00, 0x13, };
static char on_val4[] = { SPI_VAL_TYPE, 0x7F, 0xD9, };

static char on_reg5[] = { SPI_REG_TYPE, 0x00, 0x76, };
static char on_val5[] = { SPI_VAL_TYPE, 0x22, 0x13, };

static char on_reg6[] = { SPI_REG_TYPE, 0x00, 0x74, };
static char on_val6[] = { SPI_VAL_TYPE, 0x00, 0x01, };

static char on_reg7[] = { SPI_REG_TYPE, 0x00, 0x76, };
static char on_val7[] = { SPI_VAL_TYPE, 0x00, 0x00, };

static char on_reg8[] = { SPI_REG_TYPE, 0x00, 0x10, };
static char on_val8[] = { SPI_VAL_TYPE, 0x56, 0x04, };

static char on_reg9[] = { SPI_REG_TYPE, 0x00, 0x12, };
static char on_val9[] = { SPI_VAL_TYPE, 0x0C, 0x63, };

static char on_reg10[] = { SPI_REG_TYPE, 0x00, 0x01, };
static char on_val10[] = { SPI_VAL_TYPE, 0x08, 0x3B, };

static char on_reg11[] = { SPI_REG_TYPE, 0x00, 0x02, };
static char on_val11[] = { SPI_VAL_TYPE, 0x03, 0x00, };

static char on_reg12[] = { SPI_REG_TYPE, 0x00, 0x03, };
static char on_val12[] = { SPI_VAL_TYPE, 0xC0, 0x40, };

static char on_reg13[] = { SPI_REG_TYPE, 0x00, 0x08, };
static char on_val13[] = { SPI_VAL_TYPE, 0x00, 0x0A, };

static char on_reg14[] = { SPI_REG_TYPE, 0x00, 0x09, };
static char on_val14[] = { SPI_VAL_TYPE, 0x00, 0x0F, };

static char on_reg15[] = { SPI_REG_TYPE, 0x00, 0x76, };
static char on_val15[] = { SPI_VAL_TYPE, 0x22, 0x13, };

static char on_reg16[] = { SPI_REG_TYPE, 0x00, 0x0B, };
static char on_val16[] = { SPI_VAL_TYPE, 0x33, 0x40, };

static char on_reg17[] = { SPI_REG_TYPE, 0x00, 0x0C, };
static char on_val17[] = { SPI_VAL_TYPE, 0x00, 0x20, };

static char on_reg18[] = { SPI_REG_TYPE, 0x00, 0x1C, };
static char on_val18[] = { SPI_VAL_TYPE, 0x77, 0x70, };

static char on_reg19[] = { SPI_REG_TYPE, 0x00, 0x76, };
static char on_val19[] = { SPI_VAL_TYPE, 0x00, 0x00, };

static char on_reg20[] = { SPI_REG_TYPE, 0x00, 0x0D, };
static char on_val20[] = { SPI_VAL_TYPE, 0x00, 0x00, };

static char on_reg21[] = { SPI_REG_TYPE, 0x00, 0x0E, };
static char on_val21[] = { SPI_VAL_TYPE, 0x05, 0x00, };

static char on_reg22[] = { SPI_REG_TYPE, 0x00, 0x14, };
static char on_val22[] = { SPI_VAL_TYPE, 0x00, 0x00, };

static char on_reg23[] = { SPI_REG_TYPE, 0x00, 0x15, };
static char on_val23[] = { SPI_VAL_TYPE, 0x08, 0x03, };

static char on_reg24[] = { SPI_REG_TYPE, 0x00, 0x16, };
static char on_val24[] = { SPI_VAL_TYPE, 0x00, 0x00, };

static char on_reg25[] = { SPI_REG_TYPE, 0x00, 0x30, };
static char on_val25[] = { SPI_VAL_TYPE, 0x00, 0x03, };

static char on_reg26[] = { SPI_REG_TYPE, 0x00, 0x31, };
static char on_val26[] = { SPI_VAL_TYPE, 0x07, 0x0F, };

static char on_reg27[] = { SPI_REG_TYPE, 0x00, 0x32, };
static char on_val27[] = { SPI_VAL_TYPE, 0x0D, 0x05, };

static char on_reg28[] = { SPI_REG_TYPE, 0x00, 0x33, };
static char on_val28[] = { SPI_VAL_TYPE, 0x04, 0x05, };

static char on_reg29[] = { SPI_REG_TYPE, 0x00, 0x34, };
static char on_val29[] = { SPI_VAL_TYPE, 0x09, 0x0D, };

static char on_reg30[] = { SPI_REG_TYPE, 0x00, 0x35, };
static char on_val30[] = { SPI_VAL_TYPE, 0x05, 0x01, };

static char on_reg31[] = { SPI_REG_TYPE, 0x00, 0x36, };
static char on_val31[] = { SPI_VAL_TYPE, 0x04, 0x00, };

static char on_reg32[] = { SPI_REG_TYPE, 0x00, 0x37, };
static char on_val32[] = { SPI_VAL_TYPE, 0x05, 0x04, };

static char on_reg33[] = { SPI_REG_TYPE, 0x00, 0x38, };
static char on_val33[] = { SPI_VAL_TYPE, 0x0C, 0x09, };

static char on_reg34[] = { SPI_REG_TYPE, 0x00, 0x39, };
static char on_val34[] = { SPI_VAL_TYPE, 0x01, 0x0C, };

static char on_reg35[] = { SPI_REG_TYPE, 0x00, 0x07, };
static char on_val35[] = { SPI_VAL_TYPE, 0x00, 0x01, };

static char on_reg36[] = { SPI_REG_TYPE, 0x00, 0x07, };
static char on_val36[] = { SPI_VAL_TYPE, 0x01, 0x01, };

static char on_reg37[] = { SPI_REG_TYPE, 0x00, 0x07, };
static char on_val37[] = { SPI_VAL_TYPE, 0x01, 0x03, };


static char off_reg1[] = { SPI_REG_TYPE, 0x00, 0x0B, };
static char off_val1[] = { SPI_VAL_TYPE, 0x30, 0x00, };

static char off_reg2[] = { SPI_REG_TYPE, 0x00, 0x07, };
static char off_val2[] = { SPI_VAL_TYPE, 0x01, 0x02, };

static char off_reg3[] = { SPI_REG_TYPE, 0x00, 0x07, };
static char off_val3[] = { SPI_VAL_TYPE, 0x00, 0x00, };

static char off_reg4[] = { SPI_REG_TYPE, 0x00, 0x12, };
static char off_val4[] = { SPI_VAL_TYPE, 0x00, 0x00, };

static char off_reg5[] = { SPI_REG_TYPE, 0x00, 0x10, };
static char off_val5[] = { SPI_VAL_TYPE, 0x06, 0x00, };

static char off_reg6[] = { SPI_REG_TYPE, 0x00, 0x10, };
static char off_val6[] = { SPI_VAL_TYPE, 0x06, 0x01, };


static struct spi_cmd_desc display_on_cmds[] = {
	{sizeof(on_reg1), on_reg1, sizeof(on_val1), on_val1, WAIT_TYPE_MS, 10},
	{sizeof(on_reg2), on_reg2, sizeof(on_val2), on_val2, 0, 0},
	{sizeof(on_reg3), on_reg3, sizeof(on_val3), on_val3, 0, 0},
	{sizeof(on_reg4), on_reg4, sizeof(on_val4), on_val4, 0, 0},
	{sizeof(on_reg5), on_reg5, sizeof(on_val5), on_val5, 0, 0},
	{sizeof(on_reg6), on_reg6, sizeof(on_val6), on_val6, 0, 0},
	{sizeof(on_reg7), on_reg7, sizeof(on_val7), on_val7, 0, 0},
	{sizeof(on_reg8), on_reg8, sizeof(on_val8), on_val8, WAIT_TYPE_MS, 6 * 16},
	{sizeof(on_reg9), on_reg9, sizeof(on_val9), on_val9, WAIT_TYPE_MS, 5 * 16},
	{sizeof(on_reg10), on_reg10, sizeof(on_val10), on_val10, 0, 0},
	{sizeof(on_reg11), on_reg11, sizeof(on_val11), on_val11, 0, 0},
	{sizeof(on_reg12), on_reg12, sizeof(on_val12), on_val12, 0, 0},
	{sizeof(on_reg13), on_reg13, sizeof(on_val13), on_val13, 0, 0},
	{sizeof(on_reg14), on_reg14, sizeof(on_val14), on_val14, 0, 0},
	{sizeof(on_reg15), on_reg15, sizeof(on_val15), on_val15, 0, 0},
	{sizeof(on_reg16), on_reg16, sizeof(on_val16), on_val16, 0, 0},
	{sizeof(on_reg17), on_reg17, sizeof(on_val17), on_val17, 0, 0},
	{sizeof(on_reg18), on_reg18, sizeof(on_val18), on_val18, 0, 0},
	{sizeof(on_reg19), on_reg19, sizeof(on_val19), on_val19, 0, 0},
	{sizeof(on_reg20), on_reg20, sizeof(on_val20), on_val20, 0, 0},
	{sizeof(on_reg21), on_reg21, sizeof(on_val21), on_val21, 0, 0},
	{sizeof(on_reg22), on_reg22, sizeof(on_val22), on_val22, 0, 0},
	{sizeof(on_reg23), on_reg23, sizeof(on_val23), on_val23, 0, 0},
	{sizeof(on_reg24), on_reg24, sizeof(on_val24), on_val24, 0, 0},
	{sizeof(on_reg25), on_reg25, sizeof(on_val25), on_val25, 0, 0},
	{sizeof(on_reg26), on_reg26, sizeof(on_val26), on_val26, 0, 0},
	{sizeof(on_reg27), on_reg27, sizeof(on_val27), on_val27, 0, 0},
	{sizeof(on_reg28), on_reg28, sizeof(on_val28), on_val28, 0, 0},
	{sizeof(on_reg29), on_reg29, sizeof(on_val29), on_val29, 0, 0},
	{sizeof(on_reg30), on_reg30, sizeof(on_val30), on_val30, 0, 0},
	{sizeof(on_reg31), on_reg31, sizeof(on_val31), on_val31, 0, 0},
	{sizeof(on_reg32), on_reg32, sizeof(on_val32), on_val32, 0, 0},
	{sizeof(on_reg33), on_reg33, sizeof(on_val33), on_val33, 0, 0},
	{sizeof(on_reg34), on_reg34, sizeof(on_val34), on_val34, WAIT_TYPE_MS, 10},
	{sizeof(on_reg35), on_reg35, sizeof(on_val35), on_val35, WAIT_TYPE_MS, 2 * 16},
	{sizeof(on_reg36), on_reg36, sizeof(on_val36), on_val36, WAIT_TYPE_MS, 2 * 16},
	{sizeof(on_reg37), on_reg37, sizeof(on_val37), on_val37, 0, 0},
};

static struct spi_cmd_desc display_off_cmds[] = {
	{sizeof(off_reg1), off_reg1, sizeof(off_val1), off_val1, 0, 0},
	{sizeof(off_reg2), off_reg2, sizeof(off_val2), off_val2, WAIT_TYPE_MS, 2 * 16},
	{sizeof(off_reg3), off_reg3, sizeof(off_val3), off_val3, WAIT_TYPE_MS, 2 * 16},
	{sizeof(off_reg4), off_reg4, sizeof(off_val4), off_val4, 0, 0},
	{sizeof(off_reg5), off_reg5, sizeof(off_val5), off_val5, WAIT_TYPE_MS, 10},
	{sizeof(off_reg6), off_reg6, sizeof(off_val6), off_val6, WAIT_TYPE_MS, 10},
};


/*******************************************************************************
** LCD VCC
*/
#define VCC_LCDIO_NAME		"lcdio-vcc"
#define VCC_LCDANALOG_NAME	"lcdanalog-vcc"

static struct regulator *vcc_lcdio;
static struct regulator *vcc_lcdanalog;

static struct vcc_desc samsung_lcd_vcc_init_cmds[] = {
	/* vcc get */
	{DTYPE_VCC_GET, VCC_LCDIO_NAME, &vcc_lcdio, 0, 0, WAIT_TYPE_MS, 0},
	{DTYPE_VCC_GET, VCC_LCDANALOG_NAME, &vcc_lcdanalog, 0, 0, WAIT_TYPE_MS, 0},

	/* vcc set voltage */
	{DTYPE_VCC_SET_VOLTAGE, VCC_LCDANALOG_NAME, &vcc_lcdanalog, 2800000, 2800000, WAIT_TYPE_MS, 0},
};

static struct vcc_desc samsung_lcd_vcc_finit_cmds[] = {
	/* vcc put */
	{DTYPE_VCC_PUT, VCC_LCDIO_NAME, &vcc_lcdio, 0, 0, WAIT_TYPE_MS, 0},
	{DTYPE_VCC_PUT, VCC_LCDANALOG_NAME, &vcc_lcdanalog, 0, 0, WAIT_TYPE_MS, 0},
};

static struct vcc_desc samsung_lcd_vcc_enable_cmds[] = {
	/* vcc enable */
	{DTYPE_VCC_ENABLE, VCC_LCDIO_NAME, &vcc_lcdio, 0, 0, WAIT_TYPE_MS, 3},
	{DTYPE_VCC_ENABLE, VCC_LCDANALOG_NAME, &vcc_lcdanalog, 0, 0, WAIT_TYPE_MS, 3},
};

static struct vcc_desc samsung_lcd_vcc_disable_cmds[] = {
	/* vcc disable */
	{DTYPE_VCC_DISABLE, VCC_LCDANALOG_NAME, &vcc_lcdanalog, 0, 0, WAIT_TYPE_MS, 3},
	{DTYPE_VCC_DISABLE, VCC_LCDIO_NAME, &vcc_lcdio, 0, 0, WAIT_TYPE_MS, 3},
};


/*******************************************************************************
** LCD GPIO
*/
#define GPIO_LCD_POWER_NAME	"gpio_lcd_power"
#define GPIO_LCD_RESET_NAME	"gpio_lcd_reset"
#define GPIO_LCD_ID0_NAME	"gpio_lcd_id0"
#define GPIO_LCD_ID1_NAME	"gpio_lcd_id1"

static uint32_t gpio_lcd_power;
static uint32_t gpio_lcd_reset;
static uint32_t gpio_lcd_id0;
static uint32_t gpio_lcd_id1;

static struct gpio_desc samsung_lcd_gpio_request_cmds[] = {
	/* power */
	{DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		GPIO_LCD_POWER_NAME, &gpio_lcd_power, 0},
	/* reset */
	{DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
	/* id0 */
	{DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		GPIO_LCD_ID0_NAME, &gpio_lcd_id0, 0},
	/* id1 */
	{DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		GPIO_LCD_ID1_NAME, &gpio_lcd_id1, 0},
};

static struct gpio_desc samsung_lcd_gpio_free_cmds[] = {
	/* reset */
	{DTYPE_GPIO_FREE, WAIT_TYPE_MS, 0,
		GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
	/* power */
	{DTYPE_GPIO_FREE, WAIT_TYPE_MS, 0,
		GPIO_LCD_POWER_NAME, &gpio_lcd_power, 0},
	/* id0 */
	{DTYPE_GPIO_FREE, WAIT_TYPE_MS, 0,
		GPIO_LCD_ID0_NAME, &gpio_lcd_id0, 0},
	/* id1 */
	{DTYPE_GPIO_FREE, WAIT_TYPE_MS, 0,
		GPIO_LCD_ID1_NAME, &gpio_lcd_id1, 0},
};

static struct gpio_desc samsung_lcd_gpio_normal_cmds[] = {
	/* id0 */
	{DTYPE_GPIO_INPUT, WAIT_TYPE_MS, 10,
		GPIO_LCD_ID0_NAME, &gpio_lcd_id0, 0},
	/* id1 */
	{DTYPE_GPIO_INPUT, WAIT_TYPE_MS, 10,
		GPIO_LCD_ID1_NAME, &gpio_lcd_id1, 0},
	/* reset */
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 10,
		GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 1},
	/* power */
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 10,
		GPIO_LCD_POWER_NAME, &gpio_lcd_power, 1},
	/* reset */
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 10,
		GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 10,
		GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 1},
};

static struct gpio_desc samsung_lcd_gpio_lowpower_cmds[] = {
	/* id0 */
	{DTYPE_GPIO_INPUT, WAIT_TYPE_MS, 10,
		GPIO_LCD_ID0_NAME, &gpio_lcd_id0, 0},
	/* id1 */
	{DTYPE_GPIO_INPUT, WAIT_TYPE_MS, 10,
		GPIO_LCD_ID1_NAME, &gpio_lcd_id1, 0},
	/* reset */
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 10,
		GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
	/* power */
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 10,
		GPIO_LCD_POWER_NAME, &gpio_lcd_power, 0},
};

/*******************************************************************************
** LCD PWM GPIO
*/
#define GPIO_LCD_PWM0_NAME	"gpio_pwm0"
#define GPIO_LCD_PWM1_NAME	"gpio_pwm1"

static uint32_t gpio_pwm0 = 149;/*GPIO_18_5*/
static uint32_t gpio_pwm1 = 150;/*GPIO_18_6*/

static struct gpio_desc samsung_pwm_gpio_request_cmds[] = {
	/* pwm0 */
	{DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		GPIO_LCD_PWM0_NAME, &gpio_pwm0, 0},
	/* pwm1 */
	{DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		GPIO_LCD_PWM1_NAME, &gpio_pwm1, 0},
};

static struct gpio_desc samsung_pwm_gpio_free_cmds[] = {
	/* pwm0 */
	{DTYPE_GPIO_FREE, WAIT_TYPE_MS, 0,
		GPIO_LCD_PWM0_NAME, &gpio_pwm0, 0},
	/* pwm1 */
	{DTYPE_GPIO_FREE, WAIT_TYPE_MS, 0,
		GPIO_LCD_PWM1_NAME, &gpio_pwm1, 0},
};

static struct gpio_desc samsung_pwm_gpio_normal_cmds[] = {
	/* pwm1 */
	{DTYPE_GPIO_INPUT, WAIT_TYPE_MS, 10,
		GPIO_LCD_PWM1_NAME, &gpio_pwm1, 0},
};

static struct gpio_desc samsung_pwm_gpio_lowpower_cmds[] = {
	/* pwm0 */
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 10,
		GPIO_LCD_PWM0_NAME, &gpio_pwm0, 0},
	/* pwm1 */
	{DTYPE_GPIO_INPUT, WAIT_TYPE_MS, 10,
		GPIO_LCD_PWM1_NAME, &gpio_pwm1, 0},
};


/*******************************************************************************
**
*/
static int samsung_backlight_on(struct hisi_fb_data_type *hisifd)
{
	struct hisi_panel_info *pinfo = NULL;

	BUG_ON(hisifd == NULL);
	pinfo = &(hisifd->panel_info);
	BUG_ON(pinfo == NULL);

	if (pinfo->bl_set_type & BL_SET_BY_PWM) {
		/* pwm gpio request */
		gpio_cmds_tx(samsung_pwm_gpio_request_cmds, \
			ARRAY_SIZE(samsung_pwm_gpio_request_cmds));
		/* pwm gpio normal */
		gpio_cmds_tx(samsung_pwm_gpio_normal_cmds, \
			ARRAY_SIZE(samsung_pwm_gpio_normal_cmds));

		/* backlight on */
		//hisi_pwm_set_backlight(hisifd);
	}

	return 0;
}

static int samsung_backlight_off(struct hisi_fb_data_type *hisifd)
{
	struct hisi_panel_info *pinfo = NULL;

	BUG_ON(hisifd == NULL);
	pinfo = &(hisifd->panel_info);
	BUG_ON(pinfo == NULL);

	if (pinfo->bl_set_type & BL_SET_BY_PWM) {
		/* backlight off */
		hisi_pwm_set_backlight(hisifd);

		/* pwm gpio lowpower */
		gpio_cmds_tx(samsung_pwm_gpio_lowpower_cmds, \
			ARRAY_SIZE(samsung_pwm_gpio_lowpower_cmds));
		/* pwm gpio free */
		gpio_cmds_tx(samsung_pwm_gpio_free_cmds, \
			ARRAY_SIZE(samsung_pwm_gpio_free_cmds));
	}

	return 0;
}

static int lcdc_samsung_panel_set_fastboot(struct platform_device *pdev)
{
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_panel_info *pinfo = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);
	pinfo = &(hisifd->panel_info);
	BUG_ON(pinfo == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	/* lcd vcc enable */
	vcc_cmds_tx(pdev, samsung_lcd_vcc_enable_cmds, \
		ARRAY_SIZE(samsung_lcd_vcc_enable_cmds));

	/* lcd gpio request */
	gpio_cmds_tx(samsung_lcd_gpio_request_cmds, \
		ARRAY_SIZE(samsung_lcd_gpio_request_cmds));

	if (pinfo->bl_set_type & BL_SET_BY_PWM) {
		/* pwm gpio request */
		gpio_cmds_tx(samsung_pwm_gpio_request_cmds, \
			ARRAY_SIZE(samsung_pwm_gpio_request_cmds));
	}

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return 0;
}

static int lcdc_samsung_panel_on(struct platform_device *pdev)
{
	int ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_panel_info *pinfo = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);
	pinfo = &(hisifd->panel_info);
	BUG_ON(pinfo == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	if (pinfo->lcd_init_step == LCD_INIT_POWER_ON) {
		/* lcd vcc enable */
		vcc_cmds_tx(NULL, samsung_lcd_vcc_enable_cmds, \
			ARRAY_SIZE(samsung_lcd_vcc_enable_cmds));

		pinfo->lcd_init_step = LCD_INIT_LDI_SEND_SEQUENCE;
	} else if (pinfo->lcd_init_step == LCD_INIT_LDI_SEND_SEQUENCE) {
		/* lcd gpio request */
		gpio_cmds_tx(samsung_lcd_gpio_request_cmds, \
			ARRAY_SIZE(samsung_lcd_gpio_request_cmds));
		/* lcd gpio normal */
		gpio_cmds_tx(samsung_lcd_gpio_normal_cmds, \
			ARRAY_SIZE(samsung_lcd_gpio_normal_cmds));

		/* lcd display on sequence */
		ret = spi_cmds_tx(pinfo->spi_dev, display_on_cmds,
			ARRAY_SIZE(display_on_cmds));
		if (ret != 0) {
			HISI_FB_ERR("spi_cmds_tx failed! ret=%d\n", ret);
		}

		if (hisifd->panel_info.bl_set_type & BL_SET_BY_PWM) {
			/* backlight on */
			samsung_backlight_on(hisifd);
		}
	} else {
		HISI_FB_ERR("failed to init lcd!\n");
	}

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return 0;
}

static int lcdc_samsung_panel_off(struct platform_device *pdev)
{
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisi_panel_info *pinfo = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);
	pinfo = &(hisifd->panel_info);
	BUG_ON(pinfo == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	/* backlight off */
	samsung_backlight_off(hisifd);

	/* lcd display off sequence */
	spi_cmds_tx(pinfo->spi_dev, display_off_cmds,
		ARRAY_SIZE(display_off_cmds));

	/* lcd gpio lowpower */
	gpio_cmds_tx(samsung_lcd_gpio_lowpower_cmds, \
		ARRAY_SIZE(samsung_lcd_gpio_lowpower_cmds));
	/* lcd gpio free */
	gpio_cmds_tx(samsung_lcd_gpio_free_cmds, \
		ARRAY_SIZE(samsung_lcd_gpio_free_cmds));

	/* lcd vcc disable */
	vcc_cmds_tx(NULL, samsung_lcd_vcc_disable_cmds, \
		ARRAY_SIZE(samsung_lcd_vcc_disable_cmds));

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return 0;
}

static int lcdc_samsung_panel_remove(struct platform_device *pdev)
{
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	/*BUG_ON(hisifd == NULL);*/

	if (!hisifd) {
		return 0;
	}

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	/* lcd vcc finit */
	vcc_cmds_tx(pdev, samsung_lcd_vcc_finit_cmds, \
		ARRAY_SIZE(samsung_lcd_vcc_finit_cmds));

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return 0;
}

static int lcdc_samsung_panel_set_backlight(struct platform_device *pdev)
{
	int ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;

	BUG_ON(pdev == NULL);
	hisifd = platform_get_drvdata(pdev);
	BUG_ON(hisifd == NULL);

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	if (hisifd->panel_info.bl_set_type & BL_SET_BY_PWM) {
		ret = hisi_pwm_set_backlight(hisifd);
	} else if (hisifd->panel_info.bl_set_type & BL_SET_BY_BLPWM) {
		ret = hisi_blpwm_set_backlight(hisifd);
	} else {
		;
	}

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

static struct hisi_panel_info samsung_panel_info = {0};
static struct hisi_fb_panel_data samsung_panel_data = {
	.panel_info = &samsung_panel_info,
	.set_fastboot = lcdc_samsung_panel_set_fastboot,
	.on = lcdc_samsung_panel_on,
	.off = lcdc_samsung_panel_off,
	.remove = lcdc_samsung_panel_remove,
	.set_backlight = lcdc_samsung_panel_set_backlight,
};

static int lcdc_samsung_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct hisi_panel_info *pinfo = NULL;
	struct device_node *np = NULL;

	if (hisi_fb_device_probe_defer(PANEL_LCDC)) {
		goto err_probe_defer;
	}

	HISI_FB_DEBUG("+.\n");

	np = of_find_compatible_node(NULL, NULL, DTS_COMP_LCDC_SAMSUNG_LMS350DF04);
	if (!np) {
		HISI_FB_ERR("NOT FOUND device node %s!\n", DTS_COMP_LCDC_SAMSUNG_LMS350DF04);
		goto err_return;
	}

	pdev->id = 1;
	/* init lcd info */
	pinfo = samsung_panel_data.panel_info;
	memset(pinfo, 0, sizeof(struct hisi_panel_info));
	pinfo->xres = 320;
	pinfo->yres = 480;
	pinfo->width = 55;
	pinfo->height = 98;
	pinfo->type = PANEL_LCDC;
	pinfo->orientation = LCD_PORTRAIT;
	pinfo->bpp = LCD_RGB888;
	pinfo->bgr_fmt = LCD_RGB;
	pinfo->bl_set_type = BL_SET_BY_PWM;
	pinfo->bl_min = 1;
	pinfo->bl_max = 100;
	pinfo->ifbc_type = IFBC_TYPE_NON;

	pinfo->frc_enable = 0;
	pinfo->esd_enable = 0;
	pinfo->sbl_support = 0;

	pinfo->spi_dev = g_spi_dev;

	pinfo->ldi.h_back_porch = 11;
	pinfo->ldi.h_front_porch = 4;
	pinfo->ldi.h_pulse_width = 4;
	pinfo->ldi.v_back_porch = 10;  /* 8 */
	pinfo->ldi.v_front_porch = 4;
	pinfo->ldi.v_pulse_width = 2;
	pinfo->ldi.hsync_plr = 1;
	pinfo->ldi.vsync_plr = 1;
	pinfo->ldi.pixelclk_plr = 0;
	pinfo->ldi.data_en_plr = 1;

	pinfo->pxl_clk_rate = 21 * 1000000UL;

	gpio_lcd_power = of_get_named_gpio(np, "gpios", 0);  /*GPIO_21_3*/
	gpio_lcd_reset = of_get_named_gpio(np, "gpios", 1);  /*GPIO_0_3*/
	gpio_lcd_id0 = of_get_named_gpio(np, "gpios", 2);  /*GPIO_16_7*/
	gpio_lcd_id1 = of_get_named_gpio(np, "gpios", 3);  /*GPIO_17_0*/

	/* lcd vcc init */
	ret = vcc_cmds_tx(pdev, samsung_lcd_vcc_init_cmds, \
		ARRAY_SIZE(samsung_lcd_vcc_init_cmds));
	if (ret != 0) {
		HISI_FB_ERR("LCD vcc init failed!\n");
		goto err_return;
	}

	/* alloc panel device data */
	ret = platform_device_add_data(pdev, &samsung_panel_data,
		sizeof(struct hisi_fb_panel_data));
	if (ret) {
		HISI_FB_ERR("platform_device_add_data failed!\n");
		goto err_device_put;
	}

	hisi_fb_add_device(pdev);

	HISI_FB_DEBUG("-.\n");

	return 0;

err_device_put:
	platform_device_put(pdev);
err_return:
	return ret;
err_probe_defer:
	return -EPROBE_DEFER;
}

static const struct of_device_id hisi_panel_match_table[] = {
    {
        .compatible = DTS_COMP_LCDC_SAMSUNG_LMS350DF04,
        .data = NULL,
    },
    {},
};
MODULE_DEVICE_TABLE(of, hisi_panel_match_table);

static struct platform_driver this_driver = {
	.probe = lcdc_samsung_probe,
	.remove = NULL,
	.suspend = NULL,
	.resume = NULL,
	.shutdown = NULL,
	.driver = {
		.name = "lcdc_samsung_LMS350DF04",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(hisi_panel_match_table),
	},
};

static int __init lcdc_samsung_panel_init(void)
{
	int ret = 0;

	ret = platform_driver_register(&this_driver);
	if (ret) {
		HISI_FB_ERR("platform_driver_register failed, error=%d!\n", ret);
		return ret;
	}

	return ret;
}

module_init(lcdc_samsung_panel_init);
