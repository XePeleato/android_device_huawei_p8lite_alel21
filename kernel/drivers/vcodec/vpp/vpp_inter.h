/*
 **************************************************************************************
 *
 *       Filename:   vpp_inter.h
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011-07-8 16:20:00
 *         Author:  j00140427
 *
 *       Revision:  initial draft;
 **************************************************************************************
 */
#ifndef __VPP_INTER_H__
#define __VPP_INTER_H__

#include "vpp_hal.h"
struct k3_vpp_addr{
    struct ion_client *ion_client;
    struct ion_handle *ion_handle;
};
int inter_init(void);
void inter_deinit(void);

void inter_open(HAL_LAYER_E enLayer);
void inter_close(HAL_LAYER_E enLayer);

int inter_start(VPP_CONFIG_S* pVppConfig);
//add for acm
void inter_acm_boundcheck(HAL_ACMBLKINFO_S *psgl);
void inter_init_acminfo(HAL_ACMINFO_S * pacm_info, unsigned int u32TypeValue);
int inter_acm_infocheck(HAL_ACMINFO_S * pacm_info, int num_a, int num_b);
//add for pulldown
void inter_set_pddef (HAL_LAYER_E enLayer);
#endif  /* __VPP_INTER_H__ */

