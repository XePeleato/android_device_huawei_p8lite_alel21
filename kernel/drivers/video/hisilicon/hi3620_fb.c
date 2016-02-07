/*
 * Framebuffer driver of Hisilicon Hi3620 SoC
 *
 * Copyright (c) 2013 Linaro Ltd.
 * Copyright (c) 2013 Hisilicon Ltd.
 *
 * Author: 	Haojian Zhuang <haojian.zhuang@linaro.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <linux/module.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/fb.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/pm.h>
#include <linux/regulator/consumer.h>
#include <linux/mfd/r63306.h>
#include <video/of_display_timing.h>
#include <video/display_timing.h>
#include "hi3620_fb.h"

static unsigned int hi3620fb_pseudo_palette[16] = {
	0,    ~0UL, ~0UL, ~0UL, ~0UL, ~0UL, ~0UL, ~0UL,
	~0UL, ~0UL, ~0UL, ~0UL, ~0UL, ~0UL, ~0UL, ~0UL,
};

static int match_fmt_555(struct fb_var_screeninfo *var)
{
	if (var->blue.offset == 0 && var->green.offset <= 5 &&
	    var->red.offset <= 10 && var->transp.offset <= 15 &&
	    var->blue.offset <= 5 && var->green.length <= 5 &&
	    var->red.length <= 5)
		return 1;
	return 0;
}

static int match_fmt_565(struct fb_var_screeninfo *var)
{
	if (var->blue.offset == 0 && var->green.offset <= 5 &&
	    var->red.offset <= 11 && var->transp.offset == 0 &&
	    var->blue.length <= 5 && var->green.length <= 6 &&
	    var->red.length <= 5 && var->transp.length == 0)
		return 1;
	return 0;
}

static int match_fmt_888(struct fb_var_screeninfo *var)
{
	if (var->blue.offset == 0 && var->green.offset <= 8 &&
	    var->red.offset <= 16 && var->transp.offset <= 24 &&
	    var->blue.length <= 8 && var->green.length <= 8 &&
	    var->red.length <= 8)
		return 1;
	return 0;
}

static int find_best_pix_fmt(struct fb_var_screeninfo *var)
{
	if (var->bits_per_pixel == 16) {
		/* RGB565/RGBA5551/RGBX5551 */
		if (match_fmt_555(var)) {
			if (var->transp.length == 1)
				return IMG_PIXEL_FORMAT_ARGB1555; 
			else if (var->transp.length == 0)
				return IMG_PIXEL_FORMAT_RGB555;
		} else if (match_fmt_565(var))
			return IMG_PIXEL_FORMAT_RGB565;
	} else if (var->bits_per_pixel == 32) {
		if (match_fmt_888(var)) {
			if (var->transp.length == 8)
				return IMG_PIXEL_FORMAT_ARGB8888;
			else if (var->transp.length == 0)
				return IMG_PIXEL_FORMAT_RGB888;
		}
	}
	return -EINVAL;
}

static void set_pix_fmt(struct hi3620fb_info *info, int pix_fmt)
{
	struct fb_info *fb = info->fb;
	struct fb_var_screeninfo *var = &fb->var;

	switch (pix_fmt) {
	case IMG_PIXEL_FORMAT_RGB565:
		var->blue.offset = 0;
		var->blue.length = 5;
		var->green.offset = 5;
		var->green.length = 6;
		var->red.offset = 11;
		var->red.length = 5;
		var->transp.offset = 0;
		var->transp.length = 0;
		var->bits_per_pixel = 16;
		break;
	case IMG_PIXEL_FORMAT_ARGB1555:
		var->blue.offset = 0;
		var->blue.length = 5;
		var->green.offset = 5;
		var->green.length = 5;
		var->red.offset = 10;
		var->red.length = 5;
		var->transp.offset = 15;
		var->transp.length = 1;
		var->bits_per_pixel = 16;
		break;
	case IMG_PIXEL_FORMAT_RGB555:
		var->blue.offset = 0;
		var->blue.length = 5;
		var->green.offset = 5;
		var->green.length = 5;
		var->red.offset = 10;
		var->red.length = 5;
		var->transp.offset = 15;
		var->transp.length = 0;
		var->bits_per_pixel = 16;
		break;
	case IMG_PIXEL_FORMAT_ARGB8888:
		var->blue.offset = 0;
		var->blue.length = 8;
		var->green.offset = 8;
		var->green.length = 8;
		var->red.offset = 16;
		var->red.length = 8;
		var->transp.offset = 24;
		var->transp.length = 8;
		var->bits_per_pixel = 32;
		break;
	case IMG_PIXEL_FORMAT_RGB888:
		var->blue.offset = 0;
		var->blue.length = 8;
		var->green.offset = 8;
		var->green.length = 8;
		var->red.offset = 16;
		var->red.length = 8;
		var->transp.offset = 24;
		var->transp.length = 0;
		var->bits_per_pixel = 32;
		break;
	default:
		return;
	}
	info->pix_fmt = pix_fmt;
}

static int hi3620fb_check_var(struct fb_var_screeninfo *var,
			      struct fb_info *fb)
{
	struct hi3620fb_info *info = fb->par;
	int pix_fmt;

	/*
	 * Determine which pixel format we're going to use.
	 */
	pix_fmt = find_best_pix_fmt(var);
	if (pix_fmt < 0)
		return pix_fmt;
	set_pix_fmt(info, pix_fmt);

	/*
	 * Basic geometry sanity checks.
	 */
	if (var->xoffset + var->xres > var->xres_virtual)
		return -EINVAL;
	if (var->yoffset + var->yres > var->yres_virtual)
		return -EINVAL;
	if (var->xres + var->right_margin +
	    var->hsync_len + var->left_margin > 2048)
		return -EINVAL;
	if (var->yres + var->lower_margin +
	    var->vsync_len + var->upper_margin > 2048)
		return -EINVAL;

	/*
	 * Check size of framebuffer.
	 */
	if (var->xres_virtual * var->yres_virtual *
	    (var->bits_per_pixel >> 3) > fb->fix.smem_len)
		return -EINVAL;
	return 0;
}

/* It's used to make EDC configuration work. */
static void update_edc(void __iomem *base)
{
	unsigned int data;

	data = readl_relaxed(base + EDC_DISP_CTL);
	writel_relaxed(data | EDC_CTRL_CFG_OK, base + EDC_DISP_CTL);
	data &= ~EDC_CTRL_CFG_OK;
	writel_relaxed(data, base + EDC_DISP_CTL);
}

static void disable_edc(void __iomem *base)
{
	unsigned int data;

	data = readl_relaxed(base + LDI_CTRL);
	data &= ~(LDI_CTRL_EN | LDI_CTRL_SHUTDOWN);
	writel_relaxed(data, base + LDI_CTRL);
	data = readl_relaxed(base + EDC_DISP_CTL);
	data &= ~EDC_CTRL_EN;
	data |= EDC_CTRL_CLK_EN;
	writel_relaxed(data, base + EDC_DISP_CTL);
	update_edc(base);
	data = readl_relaxed(base + EDC_VIDEO_CHAN_CTRL);
	data &= ~EDC_VIDEO_CHAN_CTRL_EN;
	writel_relaxed(data, base + EDC_VIDEO_CHAN_CTRL);
}

static void enable_edc(void __iomem *base)
{
	unsigned int data;

	data = readl_relaxed(base + EDC_VIDEO_CHAN_CTRL);
	data |= EDC_VIDEO_CHAN_CTRL_EN;
	writel_relaxed(data, base + EDC_VIDEO_CHAN_CTRL);

	data = readl_relaxed(base + LDI_WORK_MODE);
	data |= LDI_WORK_MODE_EN;
	writel_relaxed(data, base + LDI_WORK_MODE);

	data = readl_relaxed(base + LDI_CTRL);
	data &= ~(LDI_CTRL_SHUTDOWN | LDI_CTRL_COLOR_MODE | LDI_CTRL_BGR);
	data &= ~(LDI_CTRL_DISP_MODE);
	data |= LDI_CTRL_EN | LDI_CTRL_DATA_GATE_EN;
	writel_relaxed(data, base + LDI_CTRL);

	data = readl_relaxed(base + EDC_DISP_CTL);
	data |= EDC_CTRL_EN | EDC_CTRL_CLK_EN;
	data &= ~(EDC_CTRL_CFG_OK_SEL | EDC_CTRL_FRAME_END_START);
	/* unflow level */
	data &= ~(0xfff << 11);
	data |= (0xc00 << 11);
	writel_relaxed(data, base + EDC_DISP_CTL);
	update_edc(base);
}

static void set_panel_control(struct fb_info *fb)
{
	struct fb_videomode *fb_vm = fb->mode;
	struct hi3620fb_info *info = fb->par;
	void __iomem *base = info->reg_base;
	u32 ldi, dpi;

	ldi = readl_relaxed(base + LDI_PLR_CTRL) & ~LDI_POLARITY_MASK;
	dpi = readl_relaxed(base + DSI_DPI_CFG) & ~DSI_DPI_POLARITY_MASK;
	if (fb_vm->sync & FB_SYNC_HOR_HIGH_ACT) {
		ldi &= ~LDI_HSYNC_POLARITY;
		dpi &= ~DSI_DPI_HSYNC_POLARITY;
	} else {
		ldi |= LDI_HSYNC_POLARITY;
		dpi |= DSI_DPI_HSYNC_POLARITY;
	}
	if (fb_vm->sync & FB_SYNC_VERT_HIGH_ACT) {
		ldi &= ~LDI_VSYNC_POLARITY;
		dpi &= ~DSI_DPI_VSYNC_POLARITY;
	} else {
		ldi |= LDI_VSYNC_POLARITY;
		dpi |= DSI_DPI_VSYNC_POLARITY;
	}
	if (fb_vm->flag & FB_FLAG_DE_HIGH) {
		ldi &= ~LDI_DATAEN_POLARITY;
		dpi &= ~DSI_DPI_DATAEN_POLARITY;
	}
	if (fb_vm->flag & FB_FLAG_DE_LOW) {
		ldi |= LDI_DATAEN_POLARITY;
		dpi |= DSI_DPI_DATAEN_POLARITY;
	}
	if (fb_vm->flag & FB_FLAG_PIXDATA_POSEDGE)
		ldi |= LDI_PIXELCLK_POLARITY;
	if (fb_vm->flag & FB_FLAG_PIXDATA_NEGEDGE)
		ldi &= ~LDI_PIXELCLK_POLARITY;
	writel_relaxed(ldi, base + LDI_PLR_CTRL);
	/* always set color mode & shutdown high active */
	writel_relaxed(dpi, info->reg_base + DSI_DPI_CFG);
}

static void set_screen_dimensions(struct fb_info *fb)
{
	struct fb_var_screeninfo *var = &fb->var;
	struct hi3620fb_info *info = fb->par;
	void __iomem *base = info->reg_base;
	u32 data;

	data = (var->left_margin & 0xfff) << 20;
	data |= var->right_margin & 0xfff;
	writel_relaxed(data, base + LDI_HRZ_CTRL0);
	data = (var->hsync_len - 1) & 0xfff;
	writel_relaxed(data, base + LDI_HRZ_CTRL1);
	data = (var->upper_margin & 0xfff) << 20;
	data |= var->lower_margin & 0xfff;
	writel_relaxed(data, base + LDI_VRT_CTRL0);
	data = (var->vsync_len - 1) & 0xfff;
	writel_relaxed(data, base + LDI_VRT_CTRL1);

	data = (var->xres - 1) & 0xfff;
	data |= ((var->yres - 1) & 0xfff) << 20;
	writel_relaxed(data, base + LDI_DSP_SIZE);

	data = (var->yres - 1) & 0xfff;
	data |= ((var->xres - 1) & 0xfff) << 16;
	writel_relaxed(data, base + EDC_DISP_SIZE);

	data = (fb->var.xres_virtual - 1) << 16 | (fb->var.yres_virtual - 1);
	writel_relaxed(data, base + EDC_VIDEO_CHAN_SIZE);
}

static void set_graphics_start(struct fb_info *fb, int xoffs, int yoffs)
{
	struct hi3620fb_info *info = fb->par;
	struct fb_var_screeninfo *var = &fb->var;
	void __iomem *base = info->reg_base;
	u32 addr, data;

	if (yoffs >= var->yres)
		data = (yoffs - var->yres) & 0xfff;
	else
		data = yoffs & 0xfff;
	data |= (xoffs & 0xfff) << 16;
	writel_relaxed(data, base + EDC_VIDEO_CHAN_XY);
	/* setup dma address */
	addr = (yoffs * var->xres_virtual + xoffs) * var->bits_per_pixel / 8;
	addr = ALIGN(addr + fb->fix.smem_start, 64);
	writel_relaxed(addr, base + EDC_VIDEO_CHAN_ADDR);
}

static void set_dma_control(struct fb_info *fb)
{
	struct hi3620fb_info *info = fb->par;
	void __iomem *base = info->reg_base;
	unsigned int data;

	/* setup dma stride */
	writel_relaxed(fb->fix.line_length, base + EDC_VIDEO_CHAN_STRIDE);

	/* setup outstanding */
	data = readl_relaxed(base + EDC_DISP_CTL);
	data &= ~(0xf << 26);
	data |= (8 << 26);
	writel_relaxed(data, base + EDC_DISP_CTL);
}

static void set_color(struct fb_info *fb)
{
	struct fb_var_screeninfo *var = &fb->var;
	struct hi3620fb_info *info = fb->par;
	void __iomem *base = info->reg_base;
	unsigned int ctrl = 0;

	ctrl = readl_relaxed(base + EDC_VIDEO_CHAN_CTRL);
	if (var->blue.offset)
		ctrl |= EDC_CHAN_CTRL_BGR;	/* BGR format */
	ctrl &= ~(7 << 16);
	switch (info->pix_fmt) {
	case IMG_PIXEL_FORMAT_ARGB1555:
	case IMG_PIXEL_FORMAT_RGB555:
		break;
	case IMG_PIXEL_FORMAT_RGB565:
		ctrl |= 1 << 16;
		break;
	case IMG_PIXEL_FORMAT_RGB888:
		ctrl |= 2 << 16;
		break;
	case IMG_PIXEL_FORMAT_ARGB8888:
		ctrl |= 3 << 16;
		break;
	}
	writel_relaxed(ctrl, base + EDC_VIDEO_CHAN_CTRL);
	ctrl = readl_relaxed(base + EDC_DISP_CTL);
	ctrl &= ~(3 << 6);
	switch (info->pix_fmt) {
	case IMG_PIXEL_FORMAT_ARGB1555:
	case IMG_PIXEL_FORMAT_RGB555:
	case IMG_PIXEL_FORMAT_RGB565:
		break;
	case IMG_PIXEL_FORMAT_RGB888:
	case IMG_PIXEL_FORMAT_ARGB8888:
		ctrl |= 2 << 6;
		break;
	}
	writel_relaxed(ctrl, base + EDC_DISP_CTL);
	ctrl = readl_relaxed(base + LDI_CTRL);
	ctrl &= ~(3 << 3);
	switch (info->pix_fmt) {
	case IMG_PIXEL_FORMAT_ARGB1555:
	case IMG_PIXEL_FORMAT_RGB555:
	case IMG_PIXEL_FORMAT_RGB565:
		break;
	case IMG_PIXEL_FORMAT_RGB888:
	case IMG_PIXEL_FORMAT_ARGB8888:
		ctrl |= 2 << 3;
		break;
	}
	writel_relaxed(ctrl, base + LDI_CTRL);
	ctrl = readl_relaxed(base + DSI_DPI_CFG);
	ctrl &= ~(7 << 2);
	switch (info->pix_fmt) {
	case IMG_PIXEL_FORMAT_ARGB1555:
	case IMG_PIXEL_FORMAT_RGB555:
	case IMG_PIXEL_FORMAT_RGB565:
		break;
	case IMG_PIXEL_FORMAT_RGB888:
	case IMG_PIXEL_FORMAT_ARGB8888:
		ctrl |= 5 << 2;
		break;
	}
	writel_relaxed(ctrl, base + DSI_DPI_CFG);
}

static int hi3620fb_set_par(struct fb_info *fb)
{
	struct fb_var_screeninfo *var = &fb->var;
	struct hi3620fb_info *info = fb->par;
	void __iomem *base = info->reg_base;
	unsigned int ctrl = 0;

	fb->fix.ypanstep = var->yres;

	ctrl = readl_relaxed(base + EDC_VIDEO_CHAN_CTRL);
	ctrl |= EDC_VIDEO_CHAN_CTRL_EN;
	writel_relaxed(ctrl, base + EDC_VIDEO_CHAN_CTRL);

	set_color(fb);
	set_panel_control(fb);
	set_screen_dimensions(fb);
	set_dma_control(fb);
	update_edc(base);
	return 0;
}

static int hi3620fb_pan_display(struct fb_var_screeninfo *var,
				struct fb_info *fb)
{
	struct hi3620fb_info *info = fb->par;
	void __iomem *base = info->reg_base;

	set_graphics_start(fb, var->xoffset, var->yoffset);
	update_edc(base);
	return 0;
}

static int hi3620fb_blank(int blank_mode, struct fb_info *info)
{
	return 0;
}

static int hi3620fb_ioctl(struct fb_info *fb, unsigned int cmd,
			  unsigned long arg)
{
	struct hi3620fb_info *info = fb->par;
	u32 count = 0, status = 0, mask = 0;
	int ret = 0;

	switch (cmd) {
	case FBIO_WAITFORVSYNC:
		count = info->vsync_cnt;

		/* clear interrupt status of End of Frame */
		status = readl_relaxed(info->reg_base + EDC_INTS);
		status &= ~EDC_INT_BAS_END;
		writel_relaxed(status, info->reg_base + EDC_INTS);
		/* unmask interrupt of End of Frame */
		mask = readl_relaxed(info->reg_base + EDC_INTE);
		mask &= ~EDC_INT_BAS_END;
		writel_relaxed(mask, info->reg_base + EDC_INTE);

		ret = wait_event_interruptible_timeout(info->wait_vsync,
						       count != info->vsync_cnt,
						       HZ / 10);
		if (ret < 0)
			return ret;
		/* mask interrupt of End of Frame */
		mask |= EDC_INT_BAS_END;
		writel_relaxed(mask, info->reg_base + EDC_INTE);
		break;
	}
	return 0;
}

static struct fb_ops hi3620fb_ops = {
	.owner		= THIS_MODULE,
	.fb_check_var	= hi3620fb_check_var,
	.fb_set_par	= hi3620fb_set_par,
	.fb_pan_display	= hi3620fb_pan_display,
	.fb_blank	= hi3620fb_blank,
	.fb_ioctl	= hi3620fb_ioctl,
	.fb_compat_ioctl = hi3620fb_ioctl,
};

static int hi3620_parse_dt(struct device_node *np, struct hi3620fb_info *info)
{
	const char *name;
	int ret;

	ret = of_property_read_u32(np, "hisilicon,dsi-clock-frequency",
				   &info->dsi_rate);
	if (ret)
		return ret;
	ret = of_property_read_string(np, "hisilicon,mipi-mode", &name);
	if (ret < 0)
		return ret;
	info->mipi_mode_name = kstrdup(name, GFP_KERNEL);
	ret = of_property_read_u32(np, "hisilicon,mipi-lanes", &info->lane_cnt);
	if (ret < 0)
		return ret;
	ret = of_property_read_u32(np, "hisilicon,color-mode", &info->color_mode);
	if (ret < 0)
		return ret;
	return 0;
}

static int hi3620_init_mode(struct device_node *np, struct fb_info *fb)
{
	struct fb_fix_screeninfo *fix = &fb->fix;
	struct fb_var_screeninfo *var = &fb->var;
	struct display_timings *disp;
	struct fb_videomode *fb_vm;
	struct hi3620fb_info *info = fb->par;
	const char *pix_name;
	int ret = 0, pix_fmt, i, length;

	fb_vm = kzalloc(sizeof(*fb_vm), GFP_KERNEL);
	if (!fb_vm)
		return -ENOMEM;
	fb->mode = fb_vm;
	disp = of_get_display_timings(np);
	if (!disp)
		return -ENOENT;
	/* How to handle multiple display timings ???,
	 * add_videomode is implemented by register_framebuffer() */
	for (i = 0; i < disp->num_timings; i++) {
		ret = of_get_fb_videomode(np, fb_vm, i);
		if (ret)
			goto out;
		ret = of_property_read_string(np, "hisilicon,pixel-format",
					      &pix_name);
		if (ret)
			goto out;
		if (!strncmp(pix_name, "RGBA8888", 8))
			pix_fmt = IMG_PIXEL_FORMAT_ARGB8888;
		else if (!strncmp(pix_name, "RGBX8888", 8))
			pix_fmt = IMG_PIXEL_FORMAT_RGB888;
		else if (!strncmp(pix_name, "RGBA5551", 8))
			pix_fmt = IMG_PIXEL_FORMAT_ARGB1555;
		else if (!strncmp(pix_name, "RGBX5551", 8))
			pix_fmt = IMG_PIXEL_FORMAT_RGB555;
		else if (!strncmp(pix_name, "RGB565", 6))
			pix_fmt = IMG_PIXEL_FORMAT_RGB565;
		else {
			ret = -EINVAL;
			goto out;
		}
		
		set_pix_fmt(info, pix_fmt);
		fb_videomode_to_var(var, fb_vm);
		var->xres_virtual = fb_vm->xres;
		var->yres_virtual = fb_vm->yres * 2;	/* double buffering */
		var->grayscale = 0;
		var->accel_flags = FB_ACCEL_NONE;
		/* Now assume that video mode is only 1 in DTS. */
		//fb_add_videomode(&vm, &fb->modelist);
	}
	of_display_timings_exist(np);

	fix->type_aux = 0;
	fix->type = FB_TYPE_PACKED_PIXELS;
	fix->visual = FB_VISUAL_TRUECOLOR;
	fix->xpanstep = 1;
	fix->ypanstep = 1;
	fix->ywrapstep = 0;
	fix->mmio_start = 0;		/* No MMIO address */
	fix->mmio_len = 0;		/* No MMIO address */
	fix->accel = FB_ACCEL_NONE;	/* No hardware accelerator */

	length = var->xres_virtual * var->bits_per_pixel / 8;
	fb->fix.line_length = length;
	fb->fix.smem_len = ALIGN(fb->fix.line_length * fb->var.yres_virtual,
				 PAGE_SIZE);
	hi3620_parse_dt(np, info);
out:
	return ret;
}

static irqreturn_t edc_irq_handler(int irq, void *data)
{
	struct hi3620fb_info *info = data;
	u32 status;

	/* clear masked interrupts */
	status = readl_relaxed(info->reg_base + EDC_INTS);
	status &= ~readl_relaxed(info->reg_base + EDC_INTE) & 0x3ffff;

	if (status & EDC_INT_BAS_END) {
		status &= ~EDC_INT_BAS_END;
		info->vsync_cnt++;
		wake_up_interruptible(&info->wait_vsync);
	}
	writel_relaxed(status, info->reg_base + EDC_INTS);
	return IRQ_HANDLED;
}

static irqreturn_t ldi_irq_handler(int irq, void *data)
{
	struct hi3620fb_info *info = data;
	u32 value;

	value = readl_relaxed(info->reg_base + LDI_ORG_INT);
	writel_relaxed(value, info->reg_base + LDI_INT_CLR);
	return IRQ_HANDLED;
}

static irqreturn_t dsi_irq_handler(int irq, void *data)
{
	return IRQ_HANDLED;
}

static int hi3620_fb_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	struct hi3620fb_info *info;
	struct resource *res;
	struct fb_info *fb;
	int ret;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(&pdev->dev, "no memory resource defined\n");
		return -ENODEV;
	}

	fb = framebuffer_alloc(sizeof(*info), dev);
	if (!fb) {
		dev_err(dev, "failed to allocate framebuffer\n");
		return -ENOMEM;
	}
	fb->dev = &pdev->dev;
	info = fb->par;
	info->fb = fb;
	info->dev = &pdev->dev;
	info->irq_edc = platform_get_irq_byname(pdev, "edc");
	if (info->irq_edc < 0) {
		ret = -ENOENT;
		goto err_fb;
	}
	info->irq_ldi = platform_get_irq_byname(pdev, "ldi");
	if (info->irq_ldi < 0) {
		ret = -ENOENT;
		goto err_fb;
	}
	info->irq_dsi = platform_get_irq_byname(pdev, "dsi");
	if (info->irq_dsi < 0) {
		ret = -ENOENT;
		goto err_fb;
	}

	info->reg_base = devm_request_and_ioremap(&pdev->dev, res);
	if (!info->reg_base) {
		ret = -EADDRNOTAVAIL;
		goto err_fb;
	}
	info->vedc = devm_regulator_get(dev, "vedc");
	if (IS_ERR_OR_NULL(info->vedc)) {
		if (IS_ERR(info->vedc)) {
			dev_err(dev, "failed to get vedc regulator\n");
			info->vedc = NULL;
		}
	}
	info->clk_ldi = of_clk_get_by_name(np, "ldi");
	if (IS_ERR(info->clk_ldi)) {
		dev_err(dev, "failed to get ldi clock\n");
		ret = PTR_ERR(info->clk_ldi);
		goto err_fb;
	}
	info->clk_edc = of_clk_get_by_name(np, "edc");
	if (IS_ERR(info->clk_edc)) {
		dev_err(dev, "failed to get edc clock\n");
		ret = PTR_ERR(info->clk_edc);
		goto err_fb;
	}
	info->clk_dsi = of_clk_get_by_name(np, "dsi");
	if (IS_ERR(info->clk_dsi)) {
		dev_err(dev, "failed to get dsi clock\n");
		ret = PTR_ERR(info->clk_dsi);
		goto err_clk;
	}
	if (info->vedc)
		regulator_enable(info->vedc);
	clk_prepare_enable(info->clk_ldi);
	//clk_prepare_enable(info->clk_edc);	/* debug for keep display on after boot */

	fb->fbops = &hi3620fb_ops;
	fb->pseudo_palette = &hi3620fb_pseudo_palette;

	ret = hi3620_init_mode(np, fb);
	if (ret)
		goto err_clk;

#if 1
	fb->screen_base = dma_alloc_coherent(fb->dev, fb->fix.smem_len,
						 &info->fb_start_dma,
						 GFP_KERNEL);
	if (!fb->screen_base) {
		dev_err(dev, "failed to allocate memory\n");
		return -ENOMEM;
	}
	fb->screen_size = fb->fix.smem_len;
	fb->fix.smem_start = info->fb_start_dma;
#else
	/* debug for remapping the display region in bootloader */
	info->fb_start_dma = PAGE_ALIGN(0x35b00130 + 0x40000000);
	fb->fix.smem_start = info->fb_start_dma;
	fb->screen_size = fb->fix.smem_len;
	fb->screen_base = __va(info->fb_start_dma);
#endif
	platform_set_drvdata(pdev, info);

	disable_edc(info->reg_base);
	hi3620_mipi_init(dev);
	hi3620_mipi_enable(dev);
	set_graphics_start(fb, 0, 0);
	hi3620fb_set_par(fb);


	/* clear IRQ status & enable IRQ */
	writel_relaxed(0, info->reg_base + EDC_INTS);
	/* enable interrupts of  bus error */ 
	writel_relaxed(0x2ff, info->reg_base + EDC_INTE);
	writel_relaxed(0x4, info->reg_base + LDI_INT_EN);	/* disable front porch int for debugging */
	writel_relaxed(0x3fff, info->reg_base + LDI_INT_CLR);

	enable_edc(info->reg_base);

	ret = devm_request_irq(dev, info->irq_edc, edc_irq_handler,
			       IRQF_DISABLED, "edc", info);
	if (ret < 0) {
		dev_err(dev, "failed to request edc irq\n");
		goto err_clk;
	}
	ret = devm_request_irq(dev, info->irq_ldi, ldi_irq_handler,
			       IRQF_DISABLED, "ldi", info);
	if (ret < 0) {
		dev_err(dev, "failed to request ldi irq\n");
		goto err_clk;
	}
#if 0
	ret = devm_request_irq(dev, info->irq_dsi, dsi_irq_handler,
			       IRQF_DISABLED, "dsi", info);
	if (ret < 0) {
		dev_err(dev, "failed to request dsi irq\n");
		goto err_clk;
	}
#endif
	ret = register_framebuffer(fb);
	if (ret < 0) {
		dev_err(dev, "failed to register hi3620 framebuffer\n");
		goto err_clk;
	}
	init_waitqueue_head(&info->wait_vsync);
	/* clock rate of ldi */

	return 0;
err_clk:
	clk_disable_unprepare(info->clk_edc);
	clk_disable_unprepare(info->clk_ldi);
err_fb:
	framebuffer_release(fb);
	return ret;
}

static int hi3620_fb_remove(struct platform_device *pdev)
{
	hi3620_mipi_disable(&pdev->dev);
	return 0;
}

static void dump_reg(void __iomem *base, int offset, int size)
{
	int i;
	for (i = 0; i < size; i += 4) {
		pr_err("#[0x%x]:0x%x\n", (int)base + offset + i, readl_relaxed(base + offset + i));
	}
}

static int dsi_init(void __iomem *base)
{
	writel_relaxed(1, base + DSI_PWR_UP);
	writel_relaxed(7, base + DSI_PHY_RSTZ);
	return 0;
}

static int hi3620_fb_suspend(struct device *dev)
{
	unsigned int data;
	struct hi3620fb_info *info = dev_get_drvdata(dev);
	void __iomem *base = info->reg_base;

	dsi_enter_sleep();

	info->graph_chan_addr = readl_relaxed(base + EDC_GRAPH_CHAN_ADDR);
	info->video_chan_addr = readl_relaxed(base + EDC_VIDEO_CHAN_ADDR);
	info->graph_chan_stride = readl_relaxed(base + EDC_GRAPH_CHAN_STRIDE);
	info->video_chan_stride = readl_relaxed(base + EDC_VIDEO_CHAN_STRIDE);
	info->graph_chan_xy = readl_relaxed(base + EDC_GRAPH_CHAN_XY);
	info->video_chan_xy = readl_relaxed(base + EDC_VIDEO_CHAN_XY);
	info->graph_chan_size = readl_relaxed(base + EDC_GRAPH_CHAN_SIZE);
	info->video_chan_size = readl_relaxed(base + EDC_VIDEO_CHAN_SIZE);

	info->edc_disp_size = readl_relaxed(base + EDC_DISP_SIZE);
	info->edc_disp_dpd = readl_relaxed(base + EDC_DISP_DPD);
	info->ldi_hrz_ctrl0 = readl_relaxed(base + LDI_HRZ_CTRL0);
	info->ldi_hrz_ctrl1 = readl_relaxed(base + LDI_HRZ_CTRL1);
	info->ldi_vrt_ctrl0 = readl_relaxed(base + LDI_VRT_CTRL0);
	info->ldi_vrt_ctrl1 = readl_relaxed(base + LDI_VRT_CTRL1);
	info->ldi_plr_ctrl = readl_relaxed(base + LDI_PLR_CTRL);
	info->ldi_dsp_size = readl_relaxed(base + LDI_DSP_SIZE);
	info->ldi_ctrl = readl_relaxed(base + LDI_CTRL);
	info->ldi_inte = readl_relaxed(base + LDI_INT_EN);
	info->dsi_cmd_mod_ctrl = readl_relaxed(base + DSI_CMD_MOD_CTRL);
	info->dsi_te_ctrl = readl_relaxed(base + DSI_TE_CTRL);
	info->dsi_te_hs_num = readl_relaxed(base + DSI_TE_HS_NUM);
	info->dsi_te_hs_wd = readl_relaxed(base + DSI_TE_HS_WD);
	info->dsi_te_vs_wd = readl_relaxed(base + DSI_TE_VS_WD);
	info->clkmgr_cfg = readl_relaxed(base + DSI_CLKMGR_CFG);
	info->dpi_cfg = readl_relaxed(base + DSI_DPI_CFG);
	info->pckhdl_cfg = readl_relaxed(base + DSI_PCKHDL_CFG);
	info->vid_mode_cfg = readl_relaxed(base + DSI_VID_MODE_CFG);
	info->vid_pkt_cfg = readl_relaxed(base + DSI_VID_PKT_CFG);
	info->cmd_mode_cfg = readl_relaxed(base + DSI_CMD_MODE_CFG);
	info->tmr_line_cfg = readl_relaxed(base + DSI_TMR_LINE_CFG);
	info->vtiming_cfg = readl_relaxed(base + DSI_VTIMING_CFG);
	info->to_cnt_cfg = readl_relaxed(base + DSI_TO_CNT_CFG);
	info->phy_if_cfg = readl_relaxed(base + DSI_PHY_IF_CFG);
	info->phy_if_ctrl = readl_relaxed(base + DSI_PHY_IF_CTRL);
	info->edpi_cfg = readl_relaxed(base + DSI_EDPI_CFG);
	info->lpcmd_time = readl_relaxed(base + DSI_LPCMD_TIME);

	/* disable channel */
	data = readl_relaxed(base + EDC_GRAPH_CHAN_CTRL);
	info->graph_chan_ctrl = data;
	data &= ~EDC_GRAPH_CHAN_CTRL_EN;
	writel_relaxed(data, base + EDC_GRAPH_CHAN_CTRL);
	data = readl_relaxed(base + EDC_VIDEO_CHAN_CTRL);
	info->video_chan_ctrl = data;
	data &= ~EDC_VIDEO_CHAN_CTRL_EN;
	writel_relaxed(data, base + EDC_VIDEO_CHAN_CTRL);

	/* disable all EDC interrupts */
	info->edc_inte = readl_relaxed(base + EDC_INTE);
	writel_relaxed(0xffffffff, base + EDC_INTE);
	/* clear EDC interrupt status */
	writel_relaxed(0, base + EDC_INTS);

	/* disable EDC */
	data = readl_relaxed(base + EDC_DISP_CTL);
	info->edc_disp_ctrl = data;
	data &= ~EDC_CTRL_EN;
	writel_relaxed(data, base + EDC_DISP_CTL);
	update_edc(base);
	return 0;
}

static int hi3620_fb_resume(struct device *dev)
{
	struct hi3620fb_info *info = dev_get_drvdata(dev);
	void __iomem *base = info->reg_base;

	dsi_init(base);
	hi3620_mipi_enable(info->dev);
	writel_relaxed(info->clkmgr_cfg, base + DSI_CLKMGR_CFG);
	set_graphics_start(info->fb, 0, 0);
	hi3620fb_set_par(info->fb);
	writel_relaxed(info->ldi_inte, base + LDI_INT_EN);
	writel_relaxed(0x3fff, info->reg_base + LDI_INT_CLR);

	/* clear EDC interrupt status */
	writel_relaxed(0, base + EDC_INTS);
	/* enable all EDC interrupts */
	writel_relaxed(info->edc_inte, base + EDC_INTE);

	writel_relaxed(info->ldi_ctrl, base + LDI_CTRL);

	enable_edc(base);

	/* enable channel */
	writel_relaxed(info->graph_chan_ctrl,
		       base + EDC_GRAPH_CHAN_CTRL);
	writel_relaxed(info->video_chan_ctrl,
		       base + EDC_VIDEO_CHAN_CTRL);

	dsi_exit_sleep();

	return 0;
}

static const struct dev_pm_ops hi3620_fb_pm_ops = {
	.suspend	= hi3620_fb_suspend,
	.resume		= hi3620_fb_resume,
};

static const struct of_device_id hi3620_fb_of_match[] = {
	{ .compatible = "hisilicon,hi3620-fb", },
	{},
};
MODULE_DEVICE_TABLE(of, hi3620_fb_of_match);

static struct platform_driver hi3620_fb_driver = {
	.probe	= hi3620_fb_probe,
	.remove	= hi3620_fb_remove,
	.driver	= {
		.name		= "hi3620-fb",
		.owner		= THIS_MODULE,
		.pm		= &hi3620_fb_pm_ops,
		.of_match_table	= hi3620_fb_of_match,
	},
};
module_platform_driver(hi3620_fb_driver);
