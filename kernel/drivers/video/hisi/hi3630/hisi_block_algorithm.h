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

#ifndef __HISI_BLOCK_ALGORITHM_H__
#define __HISI_BLOCK_ALGORITHM_H__

#include "hisi_dss.h"


#define HISI_BA_ERR(msg, ...)    \
	do { \
		printk(KERN_ERR "[hisi_block_algorithm]%s: "msg, __func__, ## __VA_ARGS__); \
	} \
	while (0)

typedef struct block_cfg {
	int Scl_Input_Left_Ov; /* if enable scf then set value to 3*/
	int Scl_Input_Right_Ov; /* if enable scf then set value to 3*/
	int Scl_Output_Left_Ov; /* if disable scp, set to 0 */
	int Scl_Output_Right_Ov; /* if disable scp, set to 0 */

	uint32_t Init_Acc; /* defalult set to 0 */

} block_cfg_t;

typedef struct block_detail {
	uint32_t acc_hscl; /* horizontal acc */
	uint32_t inc_hscl; /* horizontal inc */
	uint32_t inc_vscl; /* horizontal inc */
	uint32_t h_v_order; /* horizontal or vertical order */

	uint32_t input_width; /* for SCF reg */
	uint32_t input_height;

	uint32_t output_width;
	uint32_t output_height;

	int scp_head_clip; /* for SCP reg */
	int scp_tail_clip;

} block_detail_t;

int get_block_rect(dss_overlay_t *dss_overlay, uint32_t w, uint32_t h, int *block_num, dss_rect_t *block_rects[]);
int get_block_layers(dss_overlay_t *dss_overlay, dss_rect_t block_rect, dss_overlay_t *dss_overlay_block);
int rect_across_rect(dss_rect_t rect1, dss_rect_t rect2, dss_rect_t *cross_rect);


#endif
