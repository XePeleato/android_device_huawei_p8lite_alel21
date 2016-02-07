/*
 *
 * Copyright (C), 2001-2021, Huawei Tech. Co., Ltd.
 *
 *
 * File Name     : wireless_board.c
 * Version       : Hi1101
 * Author        : jwx222439
 * Created       : 2014-12-01
 * Last Modified :
 * Description   :
 * Function List :
 * History       :
 * 1.Date        : 2014-12-01
 *   Author      : jwx222439
 *   Modification: Created file
 *
 */
#ifdef __cplusplus
#if __cplusplus
     extern "C" {
#endif
#endif

/*****************************************************************************
  1 Header File Including
*****************************************************************************/
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/of.h>

#include "wireless_board.h"

/*****************************************************************************
  2 Global Variable Definition
*****************************************************************************/
BOARD_INFO_STRU g_st_boardid;
/*****************************************************************************
  3 Function Definition
*****************************************************************************/

/*****************************************************************************
 * Prototype    : get_board_info
 * Description  : get board cfg info
 * Input        : BOARD_INFO_STRU *
 * Output       : no
 * Return       : 0 means succeed, -1 means failed
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2014-12-01
 *     Author       : jwx222439
 *     Modification : Created function

*****************************************************************************/
int32 get_board_info(BOARD_INFO_STRU *pboard_info)
{
    int32 l_ret = -BOADR_EFAIL;
    const int8 *rw_sdio_name;
    const int8 *ini_file_name;
    struct device_node *np;

    np = of_find_compatible_node(NULL, NULL, DTS_COMP_HI1101_RW_NAME);
    if (NULL == np)
    {
        PS_PRINT_INFO("Unable to find %s\n",DTS_COMP_HI1101_RW_NAME);
        goto L1;
    }

    if (of_property_read_string(np, "wifi_sdio_rw", &rw_sdio_name))
    {
        PS_PRINT_INFO("%s node doesn't have wifi_sdio_rw property!\n", np->name);
        goto L1;
    }

    PS_PRINT_INFO("read from dts sdio name{%s}\n", rw_sdio_name);
    snprintf(pboard_info->rw_file_name, SDIO_RW_NAME_LEN, "%s%s", RW_SDIO_PATH, rw_sdio_name);

    if (of_property_read_string(np, "hisi_cfg_ini_file", &ini_file_name))
    {
        PS_PRINT_INFO("%s node doesn't have hisi_cfg_ini_file property!\n", np->name);
		pboard_info->ini_file_name[0] = '\0';
        goto L1;
    }

    PS_PRINT_INFO("read from dts ini file name{%s}\n", ini_file_name);
    snprintf(pboard_info->ini_file_name, INI_NAME_LEN, "%s", ini_file_name);

    l_ret = BOARD_SUCC;
L1:

    return l_ret;
}

EXPORT_SYMBOL(get_board_info);

#define DTS_RSSI_FIX "rssi_fix"
#define DTS_RSSI_FIX_TRUE "true"
uint32 get_wifi_rssi(void)
{
    struct device_node *np;
	uint32 rssi_value = 0;

    np = of_find_compatible_node(NULL, NULL, DTS_COMP_HI1101_RW_NAME);
    if (NULL == np)
    {
        PS_PRINT_INFO("Unable to find %s\n",DTS_COMP_HI1101_RW_NAME);
		return 0;
    }
    if (of_property_read_u32(np, DTS_RSSI_FIX, &rssi_value))
    {
		return 0;
    }
	return rssi_value;
}

EXPORT_SYMBOL(get_wifi_rssi);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
