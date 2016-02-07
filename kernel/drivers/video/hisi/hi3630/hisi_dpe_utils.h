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

#ifndef HISI_DPE_UTILS_H
#define HISI_DPE_UTILS_H

#include "hisi_fb.h"
#include <linux/string.h>

#define COMFORM_MAX	80
#define STARLIGHT_MAX	80
#define CHANGE_MAX	100
#define DISCOUNT_COEFFICIENT(value)  (CHANGE_MAX - value) / CHANGE_MAX
struct dss_clk_rate * get_dss_clk_rate(struct hisi_fb_data_type *hisifd);
int set_dss_clk_rate(struct hisi_fb_data_type *hisifd);


void init_dbuf(struct hisi_fb_data_type *hisifd);
void init_dpp(struct hisi_fb_data_type *hisifd);
void init_sbl(struct hisi_fb_data_type *hisifd);
void init_acm(struct hisi_fb_data_type *hisifd);
void init_xcc_and_gama(struct hisi_fb_data_type *hisifd);
void init_ifbc(struct hisi_fb_data_type *hisifd);
void init_ldi(struct hisi_fb_data_type *hisifd);
void deinit_ldi(struct hisi_fb_data_type *hisifd);
void enable_ldi(struct hisi_fb_data_type *hisifd);
void disable_ldi(struct hisi_fb_data_type *hisifd);

void enable_clk_pdp(struct hisi_fb_data_type *hisifd);
void disable_clk_pdp(struct hisi_fb_data_type *hisifd);
void enable_clk_sdp(struct hisi_fb_data_type *hisifd);
void disable_clk_sdp(struct hisi_fb_data_type *hisifd);
void enable_clk_adp(struct hisi_fb_data_type *hisifd);
void disable_clk_adp(struct hisi_fb_data_type *hisifd);
void enable_clk_common(struct hisi_fb_data_type *hisifd);
void disable_clk_common(struct hisi_fb_data_type *hisifd);
void enable_clk_panel(struct hisi_fb_data_type *hisifd);
void disable_clk_panel(struct hisi_fb_data_type *hisifd);
void enable_clk_dpe2(struct hisi_fb_data_type *hisifd);
void disable_clk_dpe2(struct hisi_fb_data_type *hisifd);
void enable_clk_rot1(struct hisi_fb_data_type *hisifd);
void disable_clk_rot1(struct hisi_fb_data_type *hisifd);
void enable_clk_axi(struct hisi_fb_data_type *hisifd);
void disable_clk_axi(struct hisi_fb_data_type *hisifd);

/* isr */
irqreturn_t dss_pdp_isr(int irq, void *ptr);
irqreturn_t dss_sdp_isr(int irq, void *ptr);
irqreturn_t dss_adp_isr(int irq, void *ptr);

int dpe_regulator_enable(struct hisi_fb_data_type *hisifd);
int dpe_regulator_disable(struct hisi_fb_data_type *hisifd);
int dpe_clk_enable(struct hisi_fb_data_type *hisifd);
int dpe_clk_disable(struct hisi_fb_data_type *hisifd);
void single_frame_update(struct hisi_fb_data_type *hisifd);
void dpe_init_ct_cscValue(void);
void dpe_store_ct_cscValue(unsigned int csc_value[]);
void dpe_update_g_comform_discount(unsigned int value);
void dpe_update_g_starlight_discount(unsigned int value);
int dpe_set_ct_cscValue(struct hisi_fb_data_type *hisifd);
ssize_t dpe_show_ct_cscValue(char *buf);
void dpe_init_led_rg_ct_cscValue(void);
void dpe_store_led_rg_ct_cscValue(unsigned int csc_value[]);
int dpe_set_led_rg_ct_cscValue(struct hisi_fb_data_type *hisifd);
ssize_t dpe_show_led_rg_ct_cscValue(char *buf);
int dpe_set_comform_ct_cscValue(struct hisi_fb_data_type *hisifd);
ssize_t dpe_show_comform_ct_cscValue(char *buf);
int dpe_set_starlight_ct_cscValue(struct hisi_fb_data_type *hisifd);
ssize_t dpe_show_starlight_ct_cscValue(char *buf);

#endif
