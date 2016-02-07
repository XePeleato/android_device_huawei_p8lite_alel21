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
#include "hisi_overlay_utils.h"


/*******************************************************************************
** handle isr
*/
irqreturn_t dss_pdp_isr(int irq, void *ptr)
{
	struct hisi_fb_data_type *hisifd = NULL;
	uint32_t isr_s1 = 0;
	uint32_t mask = 0;

	hisifd = (struct hisi_fb_data_type *)ptr;
	BUG_ON(hisifd == NULL);

	isr_s1 = inp32(hisifd->dss_base + GLB_CPU_PDP_INTS);
	outp32(hisifd->dss_base + GLB_CPU_PDP_INTS, ~0);

	if (isr_s1 & BIT_VACTIVE0_START) {
		if (hisifd->ov_vactive0_start_isr_handler)
			hisifd->ov_vactive0_start_isr_handler(hisifd);
	}

	if (isr_s1 & BIT_VSYNC) {
		if (hisifd->vsync_isr_handler) {
			hisifd->vsync_isr_handler(hisifd);
		}

		if (g_enable_ovl_optimized) {
			if (hisifd->ov_optimized)
				hisifd->ov_optimized(hisifd, DSS_OVL0);
		}

		if (hisifd->buf_sync_signal) {
			hisifd->buf_sync_signal(hisifd);
		}
	}

	if (isr_s1 & BIT_BACKLIGHT) {
		if (hisifd->sbl_isr_handler) {
			hisifd->sbl_isr_handler(hisifd);
		}
	}

	if (isr_s1 & BIT_LDI_UNFLOW) {
		if (g_debug_ldi_underflow) {
			/* ldi disable */
			set_reg(hisifd->dss_base + DSS_LDI0_OFFSET + LDI_CTRL, 0, 1, 0);
		}

		mask = inp32(hisifd->dss_base + GLB_CPU_PDP_INT_MSK);
		mask |= BIT_LDI_UNFLOW;
		outp32(hisifd->dss_base + GLB_CPU_PDP_INT_MSK, mask);

		HISI_FB_ERR("ldi underflow!\n");
	}

	return IRQ_HANDLED;
}

irqreturn_t dss_sdp_isr(int irq, void *ptr)
{
	struct hisi_fb_data_type *hisifd = NULL;
	uint32_t isr_s1 = 0;
	uint32_t mask = 0;

	hisifd = (struct hisi_fb_data_type *)ptr;
	BUG_ON(hisifd == NULL);

	isr_s1 = inp32(hisifd->dss_base + GLB_CPU_SDP_INTS);
	outp32(hisifd->dss_base + GLB_CPU_SDP_INTS, ~0);

	if (isr_s1 & BIT_VACTIVE0_START) {
		if (hisifd->ov_vactive0_start_isr_handler)
			hisifd->ov_vactive0_start_isr_handler(hisifd);
	}

	if (isr_s1 & BIT_VSYNC) {
		if (hisifd->vsync_isr_handler) {
			hisifd->vsync_isr_handler(hisifd);
		}

		if (g_enable_ovl_optimized) {
			if (hisifd->ov_optimized)
				hisifd->ov_optimized(hisifd, DSS_OVL0);
		}

		if (hisifd->buf_sync_signal) {
			hisifd->buf_sync_signal(hisifd);
		}
	}

	if (isr_s1 & BIT_LDI_UNFLOW) {
		if (g_debug_ldi_underflow) {
			/* ldi disable */
			set_reg(hisifd->dss_base + DSS_LDI1_OFFSET + LDI_CTRL, 0, 1, 0);
		}

		mask = inp32(hisifd->dss_base + GLB_CPU_SDP_INT_MSK);
		mask |= BIT_LDI_UNFLOW;
		outp32(hisifd->dss_base + GLB_CPU_SDP_INT_MSK, mask);

		HISI_FB_ERR("ldi underflow!\n");
	}

	return IRQ_HANDLED;
}

irqreturn_t dss_adp_isr(int irq, void *ptr)
{
	return IRQ_HANDLED;
}
