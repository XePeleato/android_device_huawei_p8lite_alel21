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
#if defined (CONFIG_HUAWEI_DSM)
void dss_debug_func(struct work_struct *work)
{
	struct clk *ddr_clk = NULL;
	unsigned long curr_ddr = 0;
	struct hisi_fb_data_type *hisifd = NULL;
	static u32 underflow_index = 0;
	static ktime_t underflow_timestamp[UNDERFLOW_EXPIRE_COUNT];
	s64 underflow_msecs = 0;
	static bool init_underflow_timestamp = false;
	int i;

	if (!init_underflow_timestamp) {
		underflow_timestamp[underflow_index] = ktime_get();
		underflow_index ++;
	}
	if (underflow_index == UNDERFLOW_EXPIRE_COUNT) {
		init_underflow_timestamp = true;
		underflow_timestamp[UNDERFLOW_EXPIRE_COUNT - 1] = ktime_get();
		underflow_msecs = ktime_to_ms(underflow_timestamp[UNDERFLOW_EXPIRE_COUNT - 1]) - ktime_to_ms(underflow_timestamp[0]);
		for(i = 0; i < UNDERFLOW_EXPIRE_COUNT - 1; i ++)
			underflow_timestamp[i] = underflow_timestamp[i+1];
	}

	if ((underflow_msecs <= UNDERFLOW_INTERVAL) && (underflow_msecs > 0))
		HISI_FB_INFO("abnormal, underflow times:%d, interval:%llu, expire interval:%d\n", UNDERFLOW_EXPIRE_COUNT, underflow_msecs, UNDERFLOW_INTERVAL);
	else {
		HISI_FB_INFO("normal, underflow times:%d, interval:%llu, expire interval:%d\n", UNDERFLOW_EXPIRE_COUNT, underflow_msecs, UNDERFLOW_INTERVAL);
		return;
	}

	ddr_clk = clk_get(NULL, "clk_ddrc_freq");
	if (ddr_clk) {
		curr_ddr = clk_get_rate(ddr_clk);
		clk_put(ddr_clk);
	} else
		HISI_FB_ERR("Get ddr clk failed");

	hisifd = container_of(work, struct hisi_fb_data_type, dss_debug_work);
	if (hisifd)
		dumpDssOverlay(&hisifd->ov_req);
	else
		HISI_FB_ERR("Get hisifd failed");

	HISI_FB_INFO("Current ddr is %lu\n", curr_ddr);
	if (!dsm_client_ocuppy(lcd_dclient)) {
		dsm_client_record(lcd_dclient, "ldi underflow!\n");
		dsm_client_notify(lcd_dclient, DSM_LCD_LDI_UNDERFLOW_NO);
	}

}
#endif

irqreturn_t dss_pdp_isr(int irq, void *ptr)
{
	struct hisi_fb_data_type *hisifd = NULL;
	uint32_t isr_s1 = 0;
	uint32_t isr_s2 = 0;
	uint32_t isr_s2_dpe0_mmu = 0;
	uint32_t isr_s2_dpe2_mmu = 0;
	uint32_t mask = 0;
	uint32_t isr_s1_cmp = 0;
	uint32_t vsync_isr_bit = 0;

	hisifd = (struct hisi_fb_data_type *)ptr;
	BUG_ON(hisifd == NULL);

	if (g_debug_mmu_error) {
		isr_s1_cmp = (BIT_PDP_LDI0_IRQ_CPU |
			BIT_DPE0_MMU_IRQ_CPU | BIT_DPE2_MMU_IRQ_CPU);
	} else {
		isr_s1_cmp = BIT_PDP_LDI0_IRQ_CPU;
	}

	if (is_mipi_cmd_panel(hisifd)) {
		vsync_isr_bit = BIT_LDI_TE0_PIN_INT;
	} else {
		vsync_isr_bit = BIT_VSYNC_INT;
	}

	isr_s1 = inp32(hisifd->dss_base + DSS_GLB_PDP_CPU_IRQ);
	if (isr_s1 & isr_s1_cmp) {
		isr_s2 = inp32(hisifd->dss_base + PDP_LDI_CPU_IRQ);
		outp32(hisifd->dss_base + PDP_LDI_CPU_IRQ_CLR, isr_s2);

		if (g_debug_mmu_error) {
			isr_s2_dpe0_mmu = inp32(hisifd->dss_base + DSS_DPE0_OFFSET +
				DSS_DP_CTRL_OFFSET + PDP_DPE0_MMU_CPU_IRQ);
			isr_s2_dpe2_mmu = inp32(hisifd->dss_base + DSS_DPE2_OFFSET +
				DSS_DP_CTRL_OFFSET + PDP_DPE2_MMU_CPU_IRQ);

			outp32(hisifd->dss_base + DSS_DPE0_OFFSET + DSS_DP_CTRL_OFFSET +
				PDP_DPE0_MMU_CPU_IRQ_CLR, isr_s2_dpe0_mmu);
			outp32(hisifd->dss_base + DSS_DPE2_OFFSET + DSS_DP_CTRL_OFFSET +
				PDP_DPE2_MMU_CPU_IRQ_CLR, isr_s2_dpe2_mmu);

			if ((isr_s2_dpe0_mmu & 0xFFFFF) || (isr_s2_dpe2_mmu & 0xFFFFF)) {
				/* ldi disable */
				set_reg(hisifd->dss_base + PDP_LDI_CTRL, 0, 1, 0);

				HISI_FB_ERR("mmu error: isr_s2_dpe0_mmu=0x%x, isr_s2_dpe2_mmu=0x%x!\n",
					isr_s2_dpe0_mmu, isr_s2_dpe2_mmu);
			}
		}

		if (isr_s2 & BIT_VACTIVE0_END_INT) {
			if (hisifd->mipi_dsi_bit_clk_upt_isr_handler &&
				hisifd->panel_info.dsi_bit_clk_upt_support &&
				!(isr_s2 & BIT_VACTIVE0_START_INT)) {
				hisifd->mipi_dsi_bit_clk_upt_isr_handler(hisifd);
			}

			if (hisifd->panel_info.esd_enable && hisifd->esd_hrtimer_enable) {
				if (is_mipi_cmd_panel(hisifd)) {
					if (hisifd->frame_end_wq) {
						queue_work(hisifd->frame_end_wq, &(hisifd->frame_end_work));
					}
				}
			}
		}

		if (isr_s2 & BIT_VACTIVE0_START_INT) {
			if (hisifd->ov_vactive0_start_isr_handler)
				hisifd->ov_vactive0_start_isr_handler(hisifd);
		}

		if (isr_s2 & vsync_isr_bit) {
			if (hisifd->vsync_isr_handler) {
				hisifd->vsync_isr_handler(hisifd);
			}

			if (g_enable_ovl_optimized) {
				if (hisifd->ov_optimized)
					hisifd->ov_optimized(hisifd);
			}

			if (hisifd->buf_sync_signal) {
				hisifd->buf_sync_signal(hisifd);
			}
		}

		if (isr_s2 & BIT_BACKLIGHT_INT) {
			if (hisifd->sbl_isr_handler) {
				hisifd->sbl_isr_handler(hisifd);
			}
		}

		if (isr_s2 & BIT_LDI_UNFLOW_INT) {
			if (g_debug_ldi_underflow) {
				/* ldi disable */
				set_reg(hisifd->dss_base + PDP_LDI_CTRL, 0, 1, 0);
			}

			mask = inp32(hisifd->dss_base + PDP_LDI_CPU_IRQ_MSK);
			mask |= BIT_LDI_UNFLOW_INT;
			outp32(hisifd->dss_base + PDP_LDI_CPU_IRQ_MSK, mask);

			if (hisifd->ldi_data_gate_en == 0) {
				hisifd->dss_exception.underflow_exception = 1;
#if defined (CONFIG_HUAWEI_DSM)
				if (hisifd->dss_debug_workqueue)
					queue_work(hisifd->dss_debug_workqueue, &hisifd->dss_debug_work);
#endif
				HISI_FB_ERR("ldi underflow!\n");
			}
		}
	}

	if (isr_s1 & BIT_WBE0_MMU_IRQ_CPU) {
		isr_s2 = inp32(hisifd->dss_base + DSS_DPE0_OFFSET +
			DSS_DP_CTRL_OFFSET+ PDP_WBE0_CPU_IRQ);
		outp32(hisifd->dss_base + DSS_DPE0_OFFSET +
			DSS_DP_CTRL_OFFSET + PDP_WBE0_CPU_IRQ_CLR, isr_s2);

		if (isr_s2 & BIT_WBE0_FRAME_END_CPU_CH1) {
			if (hisifd->ov_wb_isr_handler) {
				hisifd->ov_wb_isr_handler(hisifd);
			}
		}
	}

	return IRQ_HANDLED;
}

irqreturn_t dss_sdp_isr(int irq, void *ptr)
{
	struct hisi_fb_data_type *hisifd = NULL;
	uint32_t isr_s1 = 0;
	uint32_t isr_s2 = 0;
	uint32_t mask = 0;

	hisifd = (struct hisi_fb_data_type *)ptr;
	BUG_ON(hisifd == NULL);

	isr_s1 = inp32(hisifd->dss_base + DSS_GLB_SDP_CPU_IRQ);
	if (isr_s1 & BIT_SDP_LDI1_IRQ_MCU) {
		isr_s2 = inp32(hisifd->dss_base + SDP_LDI_CPU_IRQ);
		outp32(hisifd->dss_base + SDP_LDI_CPU_IRQ_CLR, isr_s2);

		if (isr_s2 & BIT_VACTIVE0_START_INT) {
			if (hisifd->ov_vactive0_start_isr_handler)
				hisifd->ov_vactive0_start_isr_handler(hisifd);
		}

		if (isr_s2 & BIT_VSYNC_INT) {
			if (hisifd->vsync_isr_handler) {
				hisifd->vsync_isr_handler(hisifd);
			}

			if (g_enable_ovl_optimized) {
				if (hisifd->ov_optimized)
					hisifd->ov_optimized(hisifd);
			}

			if (hisifd->buf_sync_signal) {
				hisifd->buf_sync_signal(hisifd);
			}
		}

		if (isr_s2 & BIT_LDI_UNFLOW_INT) {
			hisifd->dss_exception.underflow_exception = 1;

			mask = inp32(hisifd->dss_base + SDP_LDI_CPU_IRQ_MSK);
			mask |= BIT_LDI_UNFLOW_INT;
			outp32(hisifd->dss_base + SDP_LDI_CPU_IRQ_MSK, mask);

			HISI_FB_ERR("ldi underflow!\n");
		}
	}

	if (isr_s1 & BIT_WBE0_MMU_IRQ_CPU) {
		isr_s2 = inp32(hisifd->dss_base + DSS_DPE0_OFFSET +
			DSS_DP_CTRL_OFFSET+ SDP_WBE0_CPU_IRQ);
		outp32(hisifd->dss_base + DSS_DPE0_OFFSET +
			DSS_DP_CTRL_OFFSET + SDP_WBE0_CPU_IRQ_CLR, isr_s2);

		if (isr_s2 & BIT_WBE0_FRAME_END_CPU_CH1) {
			if (hisifd->ov_wb_isr_handler) {
				hisifd->ov_wb_isr_handler(hisifd);
			}
		}
	}

	return IRQ_HANDLED;
}

static void offline_write_back_isr_handler(struct hisi_fb_data_type *hisifd, uint32_t wbe_chn)
{
	dss_cmdlist_node_t * cmdlist_node, *_node_;

	BUG_ON(hisifd == NULL);
	BUG_ON(wbe_chn >= HISI_DSS_OFFLINE_MAX_NUM);

	list_for_each_entry_safe(cmdlist_node, _node_, &hisifd->offline_cmdlist_head[wbe_chn], list_node) {
		if(cmdlist_node->list_node_type == e_node_frame
				&& cmdlist_node->list_header->flag.bits.pending == 0x01){

			hisifd->offline_wb_done[wbe_chn] = cmdlist_node->list_node_flag;
			wake_up_interruptible_all(&hisifd->offline_writeback_wq[wbe_chn]);
			break;
		}
	}

	return;
}

irqreturn_t dss_adp_isr(int irq, void *ptr)
{
	struct hisi_fb_data_type *hisifd = NULL;
	char __iomem *cmd_list_base = NULL;
	uint32_t isr_s1 = 0;
	uint32_t isr_s2 = 0;
	uint32_t isr_cmdlist = 0;

	hisifd = (struct hisi_fb_data_type *)ptr;
	BUG_ON(hisifd == NULL);

	cmd_list_base = hisifd->dss_base + DSS_CMD_LIST_OFFSET;

	isr_s1 = inp32(hisifd->dss_base + DSS_GLB_OFFLINE_CPU_IRQ);
	if (isr_s1 & BIT_OFFLINE_S2_IRQ_CPU_OFF) {
		isr_s2 = inp32(hisifd->dss_base + DSS_GLB_OFFLINE_S2_CPU_IRQ);
		outp32(hisifd->dss_base + DSS_GLB_OFFLINE_S2_CPU_IRQ_CLR, isr_s2);

		if (isr_s2 & 1) {
			HISI_FB_INFO("OFFLINE last cfg ok\n");
		}
	}

	if (isr_s1 & BIT_CMDLIST_IRQ_CPU_OFF) {
		isr_s2 = inp32(cmd_list_base + CMDLIST_INTS);
		if (isr_s2 & BIT_CH4_INTS) {
			isr_cmdlist = inp32(cmd_list_base + CMDLIST_CH4_INTS);

			if(isr_cmdlist & BIT_PENDING_INTS){
				set_reg(cmd_list_base + CMDLIST_CH4_INTC, 0x1, 1, 3);
				offline_write_back_isr_handler(hisifd, WBE1_CHN0 - WBE1_CHN0);

				if (g_debug_ovl_offline_composer == 1)
					HISI_FB_INFO("cmd_list CH_4 has irq = 0x%08x \n", isr_cmdlist);
			}
		}

		if (isr_s2 & BIT_CH5_INTS) {
			isr_cmdlist = inp32(cmd_list_base + CMDLIST_CH5_INTS);

			if(isr_cmdlist & BIT_PENDING_INTS){
				set_reg(cmd_list_base + CMDLIST_CH5_INTC, 0x1, 1, 3);
				offline_write_back_isr_handler(hisifd, WBE1_CHN1 - WBE1_CHN0);

				if (g_debug_ovl_offline_composer == 1)
					HISI_FB_INFO("cmd_list CH_5 has irq = 0x%08x \n", isr_cmdlist);
			}
		}

		//HISI_FB_INFO("cmd_list has irq = 0x%08x \n", isr_s2);

		/* clear cmdlist irq */
		//outp32(cmd_list_base + CMDLIST_CH0_INTC, 0x3f);
		//outp32(cmd_list_base + CMDLIST_CH1_INTC, 0x3f);
		//outp32(cmd_list_base + CMDLIST_CH2_INTC, 0x3f);
		//outp32(cmd_list_base + CMDLIST_CH3_INTC, 0x3f);
		//outp32(cmd_list_base + CMDLIST_CH4_INTC, 0x3f);
		//outp32(cmd_list_base + CMDLIST_CH5_INTC, 0x3f);
	}

	if (isr_s1 & BIT_WBE1_MMU_IRQ_CPU_OFF) {
		isr_s2 = inp32(hisifd->dss_base + DSS_DPE3_OFFSET +
			DSS_DP_CTRL_OFFSET + OFFLINE_WBE1_CPU_IRQ);
		outp32(hisifd->dss_base + DSS_DPE3_OFFSET +
			DSS_DP_CTRL_OFFSET + OFFLINE_WBE1_CPU_IRQ_CLR, isr_s2);

		if (isr_s2 & BIT_OFFLINE_WBE1_CH0_FRM_END_CPU) {
			if (g_debug_ovl_offline_composer == 1)
				HISI_FB_INFO("WBE1 DMA CH0 frame end\n");

		}

		if (isr_s2 & BIT_OFFLINE_WBE1_CH1_FRM_END_CPU) {
			if (g_debug_ovl_offline_composer == 1)
				HISI_FB_INFO("WBE1 DMA CH1 frame end\n");

		}
	}

	return IRQ_HANDLED;
}
