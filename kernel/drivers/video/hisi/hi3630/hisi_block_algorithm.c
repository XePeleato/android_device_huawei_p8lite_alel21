/*
* Copyright (c) 2013-2014, Hisilicon Tech. Co., Ltd. All rights reserved.
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
#include "hisi_block_algorithm.h"

#define SCF_MIN_INPUT			(6) /* SCF min input pix 6x6 */
#define SCF_MIN_OUTPUT			(4) /* SCF min output pix 4x4 */

#define INC_FACTOR	(1 << 18)
#define SCF_LINE_BUF	(256)
#define RDMA_ROT_TILE_BUF	(256 - 32)
//#define WDMA_ROT_TILE_BUF	(256)

#define SCF_LINE_BUF_DPE2		(2560)
#define RDMA_ROT_TILE_BUF_DPE2		(2560)

int rect_across_rect(dss_rect_t rect1, dss_rect_t rect2, dss_rect_t *cross_rect)
{
	uint32_t center_x;
	uint32_t center_y;

	memset(cross_rect, 0x00, sizeof(dss_rect_t));

	if (rect1.w == 0 || rect1.h == 0 || rect2.w == 0 || rect2.h == 0)
		return 0;

	center_x = abs(rect2.x + rect2.w - 1 + rect2.x - (rect1.x + rect1.w - 1 + rect1.x));
	center_y = abs(rect2.y + rect2.h - 1 + rect2.y - (rect1.y + rect1.h - 1 + rect1.y));

	if ((center_x < rect2.w + rect1.w) && (center_y < rect2.h + rect1.h)) {
		/* rect cross case */
		cross_rect->x = MAX(rect1.x, rect2.x);
		cross_rect->y = MAX(rect1.y, rect2.y);
		cross_rect->w = MIN(rect1.x + rect1.w - 1,rect2.x + rect2.w - 1) - cross_rect->x + 1;
		cross_rect->h = MIN(rect1.y + rect1.h - 1,rect2.y + rect2.h - 1) - cross_rect->y + 1;

		return 1;
	}

	return 0;
}

uint32_t ScalerCapacity(uint32_t scf_linebuf, uint32_t inc, uint32_t in_limitaion, uint32_t out_limitation, block_cfg_t cfg)
{
	uint32_t EffPixelSclIn;
	uint32_t EffPixelSclOut;
	uint32_t EffPixelRDMAIn;
	uint32_t EffPixelRDMAOut;

	/* step1: linebuf limitation */
	if (inc >= INC_FACTOR) {
		/* down scale or no scale,first horizontal scale,then verticl scale.scf_linebuf already satisfied */
		EffPixelSclIn = scf_linebuf;
		EffPixelSclOut = EffPixelSclIn;
	} else {
		/* scale-up */
		EffPixelSclIn = scf_linebuf - cfg.Scl_Input_Left_Ov - cfg.Scl_Input_Right_Ov - 2;
		EffPixelSclOut = (EffPixelSclIn<<18) / inc;
	}

	/* step2: RDMA output limitation, that is SCF input limitation */
	if (in_limitaion != 0) {
		EffPixelRDMAIn = in_limitaion - cfg.Scl_Input_Left_Ov - cfg.Scl_Input_Right_Ov - 2;
		EffPixelRDMAOut = (EffPixelRDMAIn << 18) / inc;
		EffPixelSclOut = MIN(EffPixelSclOut, EffPixelRDMAOut);
	}

	/* step3: WDMA input limitation, that is SCF output limitation */
	EffPixelSclOut = EffPixelSclOut - cfg.Scl_Output_Left_Ov - cfg.Scl_Output_Right_Ov;
	if (out_limitation != 0)
		EffPixelSclOut = MIN(EffPixelSclOut, out_limitation);

	return EffPixelSclOut;
}

int GeomInputPlane_offline(int FirstTile, int LastTile, block_cfg_t cfg, uint32_t inc_hscl,
		int relative_start, int out_span, int layer_start_limit,
		int layer_span, int is_dpe2, int scf_linebuf_vertic_check,
		uint32_t *ini_acc,
		int *input_startpos,
		int *input_endpos)
{
	uint32_t EffPixelSclOut;
	uint32_t SpanInInputPlane;
	uint32_t acc_x;

	uint32_t acc = cfg.Init_Acc;

	if (FirstTile)
		*ini_acc = acc + relative_start * inc_hscl;
	else
		/* calc the Scl_Output_Left_Ov acc */
		*ini_acc = acc + relative_start * inc_hscl - cfg.Scl_Output_Left_Ov * inc_hscl;

	*input_startpos = (*ini_acc)>>18;

	if (FirstTile && LastTile)
		/* this means layer is in the block,should do something ? */
		EffPixelSclOut = out_span;
	else if (FirstTile)
		EffPixelSclOut = out_span + cfg.Scl_Output_Right_Ov;
	else if (LastTile)
		EffPixelSclOut = out_span + cfg.Scl_Output_Left_Ov;
	else
		EffPixelSclOut = out_span + cfg.Scl_Output_Right_Ov + cfg.Scl_Output_Left_Ov;

	/* end position */
	acc_x = (*ini_acc) + (EffPixelSclOut - 1) * inc_hscl;
	*input_endpos = acc_x>>18;

	/* keep decimal part acc */
	*ini_acc = (*ini_acc) & 0x3FFFF;

	if(!FirstTile)
		*ini_acc = (*ini_acc) + (cfg.Scl_Input_Left_Ov << 18);

	*input_startpos = MAX(layer_start_limit, (*input_startpos) - cfg.Scl_Input_Left_Ov);
	*input_endpos = MIN((*input_endpos) + cfg.Scl_Input_Right_Ov, layer_start_limit + layer_span - 1);

	SpanInInputPlane = (*input_endpos) - (*input_startpos) + 1;
	if(inc_hscl < INC_FACTOR &&
                SpanInInputPlane > (is_dpe2 ? SCF_LINE_BUF_DPE2 : SCF_LINE_BUF) &&
                scf_linebuf_vertic_check) {
                HISI_BA_ERR("err!! input_startpos[%d],input_endpos[%d],inc[%d],FirstTile[%d]"
			"LastTile[%d],relative_start[%d],out_span[%d]\n",
			*input_startpos, *input_endpos, inc_hscl,FirstTile,LastTile,relative_start,
			out_span);
		return -1;
	}
	return 0;
}

int layer_need_cut_block(dss_layer_t *layer)
{
	/* Dim or background layer is set in overlay*/
	if (layer->need_cap & (CAP_DIM | CAP_BASE))
		return 0;

	/* if layer has rot or vertical scale */
	if ((layer->need_cap & CAP_ROT) || (layer->src_rect.h != layer->dst_rect.h))
		return 1;

	return 0;
}

int block_algorithm_need(dss_overlay_t *dss_overlay)
{
	uint32_t i = 0;

	for (i = 0; i < dss_overlay->layer_nums; i++) {
		if (layer_need_cut_block(&dss_overlay->layer_infos[i]) != 0)
			return 1;
	}

	return 0;
}

void set_layer_cfg_info(dss_layer_t *layer_info, int need_rot,
	block_cfg_t *b_cfg, block_detail_t *b_detail)
{
	if (need_rot) {
		b_detail->inc_hscl = DSS_WIDTH(layer_info->src_rect.h) * SCF_CONST_FACTOR / DSS_WIDTH(layer_info->dst_rect.w);
		b_detail->inc_vscl = (DSS_HEIGHT(layer_info->src_rect.w) * SCF_CONST_FACTOR + SCF_CONST_FACTOR / 2 - b_detail->acc_hscl) /
					DSS_HEIGHT(layer_info->dst_rect.h);
	} else {
		b_detail->inc_hscl = DSS_WIDTH(layer_info->src_rect.w) * SCF_CONST_FACTOR / DSS_WIDTH(layer_info->dst_rect.w);
		b_detail->inc_vscl = (DSS_HEIGHT(layer_info->src_rect.h) * SCF_CONST_FACTOR + SCF_CONST_FACTOR / 2 - b_detail->acc_hscl) /
					DSS_HEIGHT(layer_info->dst_rect.h);
	}

	if (((need_rot) && (layer_info->src_rect.h != layer_info->dst_rect.w))
		|| ((!need_rot) && (layer_info->src_rect.w != layer_info->dst_rect.w))) {
		b_cfg->Scl_Input_Left_Ov = 3;
		b_cfg->Scl_Input_Right_Ov = 3;
		if (b_detail->inc_hscl > INC_FACTOR)
			b_detail->h_v_order = 0;
		else
			b_detail->h_v_order = 1;
	}
}

void construt_block_info(dss_layer_t *layer_info, block_cfg_t *b_cfg, block_detail_t *b_detail)
{
	int need_rot = (layer_info->need_cap & CAP_ROT) ? 1 : 0;

	b_cfg->Init_Acc = 0;
	/* if no scp set to 0 */
	b_cfg->Scl_Output_Left_Ov = 0;
	b_cfg->Scl_Output_Right_Ov = 0;
	/* if no horizontal inc set to 0 */
	b_cfg->Scl_Input_Left_Ov = 0;
	b_cfg->Scl_Input_Right_Ov = 0;

	b_detail->inc_hscl = INC_FACTOR;
	b_detail->inc_vscl = INC_FACTOR;
	b_detail->h_v_order = 0;

	if (layer_info->need_cap & CAP_SCP) {
		/* set head_clip and tail_clip, this may be can change */
		b_cfg->Scl_Output_Left_Ov = 3;
		b_cfg->Scl_Output_Right_Ov = 3;
	}

	if (layer_info->need_cap & CAP_SCL)
		set_layer_cfg_info(layer_info, need_rot, b_cfg, b_detail);
}

int get_inc_hscl(dss_layer_t *layer_infos)
{
	int inc_hscl = INC_FACTOR;

	if (!(layer_infos->need_cap & CAP_SCL))
		return inc_hscl;

	if (layer_infos->need_cap & CAP_ROT)
		inc_hscl = DSS_HEIGHT(layer_infos->src_rect.h) * INC_FACTOR / DSS_HEIGHT(layer_infos->dst_rect.w);
	else
		inc_hscl = DSS_WIDTH(layer_infos->src_rect.w) * INC_FACTOR / DSS_WIDTH(layer_infos->dst_rect.w);

	return inc_hscl;
}

/*lint -e666*/
uint32_t calc_dest_block_size(dss_overlay_t *dss_overlay)
{
	uint32_t i;
	uint32_t block_width = 0xFFFF;
	int in_limit = 0;
	block_cfg_t b_cfg;
	block_detail_t b_detail;
	int inc_hscl = INC_FACTOR;
	int is_dpe2 = 0;

	for (i = 0; i < dss_overlay->layer_nums; i++) {
		if (layer_need_cut_block(&dss_overlay->layer_infos[i]) == 0)
			continue;

		is_dpe2 = (dss_overlay->layer_infos[i].chn_idx <= DPE2_CHN3) ? 1 : 0;

		if (dss_overlay->layer_infos[i].need_cap & CAP_SCL) {
			inc_hscl = get_inc_hscl(&dss_overlay->layer_infos[i]);
			construt_block_info(&dss_overlay->layer_infos[i], &b_cfg, &b_detail);

			if (dss_overlay->layer_infos[i].need_cap & CAP_ROT)
				in_limit = is_dpe2 ? RDMA_ROT_TILE_BUF_DPE2 : RDMA_ROT_TILE_BUF;
			else
				in_limit = 0;
			block_width = MIN(ScalerCapacity((is_dpe2 ? SCF_LINE_BUF_DPE2 : SCF_LINE_BUF), inc_hscl, in_limit, 0, b_cfg), block_width);
		} else if (dss_overlay->layer_infos[i].need_cap & CAP_ROT) {
			/* this means only has rot and no scl*/
			block_width = MIN((is_dpe2 ? RDMA_ROT_TILE_BUF_DPE2 : RDMA_ROT_TILE_BUF), block_width);
		} else {
			HISI_BA_ERR("layer has no rot and scl, should no get here\n");
		}
	}

	if(block_width % 2 && block_width != 0xFFFF)
		block_width -= 1;

	if (g_debug_ovl_offline_composer == 1)
		HISI_BA_ERR("block_width = %d\n",block_width);

	return block_width;
}
/*lint +e666*/

int construct_block_layer_info_offline(uint32_t input_startpos, uint32_t input_width,
	dss_rect_t src_rect, dss_layer_t *layer_cut_block)
{
	dss_rect_t rect_trsform = {0};

	switch (layer_cut_block->transform) {
	case HISI_FB_TRANSFORM_NOP:
	case HISI_FB_TRANSFORM_FLIP_V:
		rect_trsform.x = src_rect.x + input_startpos;
		rect_trsform.y = src_rect.y;
		rect_trsform.w = input_width;
		rect_trsform.h = src_rect.h;
		break;
	case HISI_FB_TRANSFORM_FLIP_H:
	case HISI_FB_TRANSFORM_ROT_180:
		rect_trsform.x = src_rect.x + src_rect.w - input_startpos - input_width;
		rect_trsform.y = src_rect.y;
		rect_trsform.w = input_width;
		rect_trsform.h = src_rect.h;
		break;
	case HISI_FB_TRANSFORM_ROT_90:
	case HISI_FB_TRANSFORM_ROT_90 | HISI_FB_TRANSFORM_FLIP_H:
		rect_trsform.x = src_rect.x;
		rect_trsform.y = src_rect.y + src_rect.h - input_startpos - input_width;
		rect_trsform.w = src_rect.w;
		rect_trsform.h = input_width;
		break;
	case HISI_FB_TRANSFORM_ROT_90 | HISI_FB_TRANSFORM_FLIP_V:
	case HISI_FB_TRANSFORM_ROT_270:
		rect_trsform.x = src_rect.x;
		rect_trsform.y = src_rect.y + input_startpos;
		rect_trsform.w = src_rect.w;
		rect_trsform.h = input_width;
		break;
	default:
		HISI_BA_ERR("unknow dss_layer->transform: %d\n",layer_cut_block->transform);
		return 1;
	}

	/* modify src_rect_mask */
	rect_across_rect(rect_trsform, layer_cut_block->src_rect_mask, &layer_cut_block->src_rect_mask);

	/* modify src */
	//layer_cut_block->src.width = rect_trsform.w;
	//layer_cut_block->src.height = rect_trsform.h;

	/* modify src_rect */
	memcpy(&layer_cut_block->src_rect, &rect_trsform, sizeof(rect_trsform));

	return 0;
}

int scf_output_suitable(uint32_t x_start, uint32_t x_end, uint32_t pos)
{
	if ((x_start > pos) || (x_end < pos))
		return 0;

	if ((pos - x_start < SCF_MIN_OUTPUT) || (x_end - pos + 1 < SCF_MIN_OUTPUT))
		return 1;

	return 0;
}

void init_block_cfg(block_detail_t *b_detail)
{
	memset(b_detail, 0x00, sizeof(block_detail_t));
	b_detail->inc_hscl = INC_FACTOR;
	b_detail->inc_vscl = INC_FACTOR;
	b_detail->h_v_order = 0;
}

int block_fix_scf_constraint(dss_overlay_t *dss_overlay, uint32_t block_size,
	uint32_t end_pos, uint32_t *fix_size)
{
	uint32_t i;
	uint32_t end = end_pos;
	uint32_t scf_layer_num = 0;
	#define MAX_OFFLINE_SCF 3
	dss_rect_t scf_dst_rect[MAX_OFFLINE_SCF];

	*fix_size = block_size;

	if (block_size <= SCF_MIN_OUTPUT * MAX_OFFLINE_SCF) {
		HISI_BA_ERR("block size[%d] is too small!\n", block_size);
		return 1;
	}

	for (i = 0; i < dss_overlay->layer_nums; i++) {
		if ((dss_overlay->layer_infos[i].need_cap & CAP_SCL) == 0)
			continue;

		scf_dst_rect[scf_layer_num].x = dss_overlay->layer_infos[i].dst_rect.x;
		scf_dst_rect[scf_layer_num].y = dss_overlay->layer_infos[i].dst_rect.y;
		scf_dst_rect[scf_layer_num].w = dss_overlay->layer_infos[i].dst_rect.w;
		scf_dst_rect[scf_layer_num].h = dss_overlay->layer_infos[i].dst_rect.h;
		scf_layer_num++;

		if (scf_layer_num > MAX_OFFLINE_SCF) {
			HISI_BA_ERR("scf layer in offline [%d] is more than scf moudle [%d]\n",scf_layer_num,MAX_OFFLINE_SCF);
			return 1;
		}
	}

	if (scf_layer_num == 0)
		return 0;

REDO:
	for (i = 0; i < scf_layer_num; i++) {
		if (scf_output_suitable(scf_dst_rect[i].x,
			scf_dst_rect[i].x + scf_dst_rect[i].w - 1, end)) {
			end = end - SCF_MIN_OUTPUT;
			goto REDO;
		}
	}

	*fix_size = block_size - (end_pos - end);
	return 0;
}

int get_block_rect(dss_overlay_t *dss_overlay, uint32_t w, uint32_t h,
	int *block_num, dss_rect_t *block_rects[])
{
	int ret = 0;
	uint32_t block_size = 0xFFFF;
	uint32_t current_offset = 0;
	uint32_t last_offset = 0;
	uint32_t fix_scf_span = 0;
	dss_layer_t *dss_layer = NULL;
	uint32_t i = 0;
	int block_has_layer = 0;
	int last_block_has_layer = 1;

	*block_num = 0;

	if (!dss_overlay || !block_rects) {
		HISI_BA_ERR("invaild input\n");
		return -1;
	}

	if (!block_algorithm_need(dss_overlay)) {
		block_rects[*block_num]->x = 0;
		block_rects[*block_num]->y = 0;
		block_rects[*block_num]->w = w;
		block_rects[*block_num]->h = h;

		*block_num = 1;
		return ret;
	}

	block_size = calc_dest_block_size(dss_overlay);
	if (block_size == 0xFFFF) {
		HISI_BA_ERR("calc_dest_block_size err\n");
		return -2;
	}

	if (block_size >= w) {
		block_rects[*block_num]->x = 0;
		block_rects[*block_num]->y = 0;
		block_rects[*block_num]->w = w;
		block_rects[*block_num]->h = h;

		*block_num = 1;
		return ret;
	}

	current_offset = block_size;
	fix_scf_span = block_size;

	for (current_offset = block_size; last_offset < w; last_offset = current_offset, current_offset += block_size) {
		if (block_fix_scf_constraint(dss_overlay, block_size, current_offset, &fix_scf_span) != 0) {
			HISI_BA_ERR("block_fix_scf_constraint err\n");
			return -3;
		}
		current_offset = current_offset - (block_size - fix_scf_span);

		block_has_layer = 0;

		if (current_offset - last_offset < SCF_MIN_OUTPUT) {
			HISI_BA_ERR("scf fix size err,block_size[%d] fix_scf_span[%d],current_offset[%d] last_offset[%d]\n",
				block_size, fix_scf_span, current_offset, last_offset);
			return -4;
		}

		for (i = 0; i < dss_overlay->layer_nums; i++) {
			dss_layer = &dss_overlay->layer_infos[i];
			if ((last_offset <= dss_layer->dst_rect.x + dss_layer->dst_rect.w - 1) && (dss_layer->dst_rect.x < current_offset)) {
				block_has_layer = 1;
				last_block_has_layer = 0;

				if((*block_num) >= HISI_DSS_OFFLINE_MAX_BLOCK)
					return -5;

				block_rects[*block_num]->x = last_offset;
				block_rects[*block_num]->y = 0;
				block_rects[*block_num]->w = MIN(current_offset - last_offset, w - last_offset);
				block_rects[*block_num]->h = h;

				(*block_num)++;
				break;
			}
		}

		if (block_has_layer == 0) {
			if ((last_block_has_layer == 0) || (*block_num == 0)) {

				if((*block_num) >= HISI_DSS_OFFLINE_MAX_BLOCK)
					return -6;

				block_rects[*block_num]->x = last_offset;
				block_rects[*block_num]->y = 0;
				block_rects[*block_num]->w = MIN(current_offset - last_offset, w - last_offset);
				block_rects[*block_num]->h = h;

				(*block_num)++;
			} else {
				block_rects[*block_num - 1]->w += MIN(current_offset - last_offset, w - last_offset);
			}
			last_block_has_layer = 1;
		}
	}

	return ret;
}

int get_block_layers(dss_overlay_t *dss_overlay, dss_rect_t block_rect,
	dss_overlay_t *dss_overlay_block)
{
	uint32_t i;
	int status = 0;
	int block_layer_num = 0;
	int FirstTile = 0;
	int LastTile = 0;
	int layer_index = 0;
	int input_startpos = 0;
	int input_endpos = 0;
	int is_dpe2 = 0;

	block_cfg_t b_cfg;
	block_detail_t b_detail;
	uint32_t output_startpos;  /* relative to overlay plane */
	uint32_t output_span;
	dss_layer_t *dss_layer = NULL;
	dss_rect_t dst_cross_rect;
	uint32_t src_layer_span;
	uint32_t acc_hscl = 0;

	/* if layer has no vertical scf, then horizontal
	   scf input can larger than scf_line_buf
	*/
	int scf_linebuf_vertic_check = 1;

	if (!dss_overlay_block || !block_rect.w || !block_rect.h) {
		HISI_BA_ERR("invaild args\n");
		return -1;
	}

	if (!block_algorithm_need(dss_overlay)) {
		/* must all layer in the block_rect*/
		memcpy(dss_overlay_block, dss_overlay, sizeof(dss_overlay_t));
		return 0;
	}

	/* do some init */
	memcpy(&dss_overlay_block->wb_layer_info, &dss_overlay->wb_layer_info, sizeof(dss_wb_layer_t));
	dss_overlay_block->layer_nums = 0;
	//dss_overlay_block->mmu_enable = dss_overlay->mmu_enable;

	for (i = 0; i < dss_overlay->layer_nums; i++) {
		dss_layer = &dss_overlay->layer_infos[i];
		block_layer_num = dss_overlay_block->layer_nums;
		status = rect_across_rect(dss_layer->dst_rect, block_rect, &dst_cross_rect);
		if (status == 0)
			continue;

		output_startpos = dst_cross_rect.x - dss_layer->dst_rect.x;
		output_span = dst_cross_rect.w;
		if ((output_span < SCF_MIN_OUTPUT) && (dss_layer->need_cap & CAP_SCL)) {
			HISI_BA_ERR("block layer[%d] scf output smaller than SCF_MIN_OUTPUT, dump some info\
				dss_layer dst{%d,%d,%d,%d} block_rect{%d,%d,%d,%d}\n",
			dss_layer->chn_idx,
			dss_layer->dst_rect.x,dss_layer->dst_rect.y,
			dss_layer->dst_rect.w,dss_layer->dst_rect.h,
			block_rect.x,block_rect.y,
			block_rect.w,block_rect.h);
			return -2;
		}

		FirstTile = (dss_layer->dst_rect.x >= block_rect.x) ? 1 : 0;
		LastTile = (block_rect.x + block_rect.w > dss_layer->dst_rect.x + dss_layer->dst_rect.w) ? 1 : 0;

		memcpy(&dss_overlay_block->layer_infos[block_layer_num],
			dss_layer, sizeof(dss_layer_t));

		dss_overlay_block->layer_nums++;
		dss_overlay_block->layer_infos[block_layer_num].block_info.first_tile = FirstTile;
		dss_overlay_block->layer_infos[block_layer_num].block_info.last_tile = LastTile;
		memcpy(&dss_overlay_block->layer_infos[block_layer_num].dst_rect,
			&dst_cross_rect, sizeof(dss_rect_t));

		dss_overlay_block->layer_infos[block_layer_num].layer_idx = layer_index++;

		init_block_cfg(&b_detail);
		if (dss_layer->need_cap & CAP_SCL) {
			construt_block_info(dss_layer, &b_cfg, &b_detail);
			if (dss_layer->need_cap & CAP_ROT) {
				src_layer_span = dss_layer->src_rect.h;
                                scf_linebuf_vertic_check = !(dss_layer->src_rect.w == dss_layer->dst_rect.h);
                        } else {
                                src_layer_span = dss_layer->src_rect.w;
                                scf_linebuf_vertic_check = !(dss_layer->src_rect.h == dss_layer->dst_rect.h);
                        }

			is_dpe2 = (dss_layer->chn_idx <= DPE2_CHN3) ? 1 : 0;
			if (GeomInputPlane_offline(FirstTile, LastTile, b_cfg, b_detail.inc_hscl,
					output_startpos, output_span, 0, src_layer_span, is_dpe2, scf_linebuf_vertic_check,
					&acc_hscl, &input_startpos, &input_endpos) != 0)
				return -3;

			dss_overlay_block->layer_infos[block_layer_num].block_info.acc_hscl = acc_hscl;
		} else {
			input_startpos = output_startpos;
			input_endpos = output_startpos + output_span - 1;
		}

		dss_overlay_block->layer_infos[block_layer_num].block_info.h_v_order = b_detail.h_v_order;
		dss_overlay_block->layer_infos[block_layer_num].block_info.h_ratio = b_detail.inc_hscl;
		dss_overlay_block->layer_infos[block_layer_num].block_info.v_ratio = b_detail.inc_vscl;

		construct_block_layer_info_offline(input_startpos,
			input_endpos - input_startpos + 1,
			dss_layer->src_rect,
			&dss_overlay_block->layer_infos[block_layer_num]);
	}

	return 0;
}
