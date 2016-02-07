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

#include "hisi_mipi_dsi.h"


/*
 * mipi dsi short write with 0, 1 2 parameters
 * Write to GEN_HDR 24 bit register the value:
 * 1. 00h, MCS_command[15:8] ,VC[7:6],13h
 * 2. Data1[23:16], MCS_command[15:8] ,VC[7:6],23h
 */
int mipi_dsi_swrite(struct dsi_cmd_desc *cm, char __iomem *dsi_base)
{
	uint32_t hdr = 0;
	int len = 0;

	if (cm->dlen && cm->payload == 0) {
		HISI_FB_ERR("NO payload error!\n");
		return 0;
	}

	len = (cm->dlen > 2) ? 2 : cm->dlen;

	hdr |= DSI_HDR_DTYPE(cm->dtype);
	hdr |= DSI_HDR_VC(cm->vc);
	if (len == 1) {
		hdr |= DSI_HDR_DATA1(cm->payload[0]);
		hdr |= DSI_HDR_DATA2(0);
	} else if (len == 2) {
		hdr |= DSI_HDR_DATA1(cm->payload[0]);
		hdr |= DSI_HDR_DATA2(cm->payload[1]);
	} else {
		hdr |= DSI_HDR_DATA1(0);
		hdr |= DSI_HDR_DATA2(0);
	}

	set_reg(dsi_base + MIPIDSI_GEN_HDR_OFFSET, hdr, 24, 0);

	return len;  /* 4 bytes */
}

/*
 * mipi dsi long write
 * Write to GEN_PLD_DATA 32 bit register the value:
 * Data3[31:24], Data2[23:16], Data1[15:8], MCS_command[7:0]
 * If need write again to GEN_PLD_DATA 32 bit register the value:
 * Data7[31:24], Data6[23:16], Data5[15:8], Data4[7:0]
 *
 * Write to GEN_HDR 24 bit register the value: WC[23:8] ,VC[7:6],29h
 */
int mipi_dsi_lwrite(struct dsi_cmd_desc *cm, char __iomem *dsi_base)
{
	uint32_t hdr = 0;
	int i = 0;

	if (cm->dlen && cm->payload == 0) {
		HISI_FB_ERR("NO payload error!\n");
		return 0;
	}

	/* fill up payload */
	for (i = 0;  i < cm->dlen; i += 4) {
		set_reg(dsi_base + MIPIDSI_GEN_PLD_DATA_OFFSET, *((uint32_t *)(cm->payload + i)), 32, 0);
	}

	/* fill up header */
	hdr |= DSI_HDR_DTYPE(cm->dtype);
	hdr |= DSI_HDR_VC(cm->vc);
	hdr |= DSI_HDR_WC(cm->dlen);
	set_reg(dsi_base + MIPIDSI_GEN_HDR_OFFSET, hdr, 24, 0);

	return cm->dlen;
}

void mipi_dsi_max_return_packet_size(struct dsi_cmd_desc *cm, char __iomem *dsi_base)
{
	uint32_t hdr = 0;

	/* fill up header */
	hdr |= DSI_HDR_DTYPE(cm->dtype);
	hdr |= DSI_HDR_VC(cm->vc);
	hdr |= DSI_HDR_WC(cm->dlen);
	set_reg(dsi_base + MIPIDSI_GEN_HDR_OFFSET, hdr, 24, 0);
}

uint32_t mipi_dsi_read(uint32_t *out, char __iomem *dsi_base)
{
	uint32_t pkg_status;
	uint32_t try_times = 30;

	do {
		pkg_status = inp32(dsi_base + MIPIDSI_CMD_PKT_STATUS_OFFSET);
		if (!(pkg_status & 0x10))
			break;
		msleep(1);
	} while (--try_times);

	*out = inp32(dsi_base + MIPIDSI_GEN_PLD_DATA_OFFSET);
	if (!try_times)
		HISI_FB_ERR("timeout\n");

	return try_times;
}

void mipi_dsi_sread(uint32_t *out, char __iomem *dsi_base)
{
	unsigned long dw_jiffies = 0;
	uint32_t tmp = 0;

	dw_jiffies = jiffies + HZ / 2;
	do {
		tmp = inp32(dsi_base + MIPIDSI_CMD_PKT_STATUS_OFFSET);
		if ((tmp & 0x00000040) == 0x00000040) {
			break;
		}
	} while (time_after(dw_jiffies, jiffies));

	dw_jiffies = jiffies + HZ / 2;
	do {
		tmp = inp32(dsi_base + MIPIDSI_CMD_PKT_STATUS_OFFSET);
		if ((tmp & 0x00000040) != 0x00000040) {
			break;
		}
	} while (time_after(dw_jiffies, jiffies));

	*out = inp32(dsi_base + MIPIDSI_GEN_PLD_DATA_OFFSET);
}

void mipi_dsi_lread(uint32_t *out, char __iomem *dsi_base)
{
	/* do something here*/
}

/*
 * prepare cmd buffer to be txed
 */
int mipi_dsi_cmd_add(struct dsi_cmd_desc *cm, char __iomem *dsi_base)
{
	int len = 0;

	BUG_ON(cm == NULL);
	BUG_ON(dsi_base == NULL);

	switch (cm->dtype) {
	case DTYPE_GEN_WRITE:
	case DTYPE_GEN_WRITE1:
	case DTYPE_GEN_WRITE2:

	case DTYPE_DCS_WRITE:
	case DTYPE_DCS_WRITE1:
		len = mipi_dsi_swrite(cm, dsi_base);
		break;
	case DTYPE_GEN_LWRITE:
	case DTYPE_DCS_LWRITE:
		len = mipi_dsi_lwrite(cm, dsi_base);
		break;
	default:
		HISI_FB_ERR("dtype=%x NOT supported!\n", cm->dtype);
		break;
	}

	return len;
}

int mipi_dsi_cmds_tx(struct dsi_cmd_desc *cmds, int cnt, char __iomem *dsi_base)
{
	struct dsi_cmd_desc *cm = NULL;
	int i = 0;

	BUG_ON(cmds == NULL);
	BUG_ON(dsi_base == NULL);

	cm = cmds;

	for (i = 0; i < cnt; i++) {
		mipi_dsi_cmd_add(cm, dsi_base);

		if (cm->wait) {
			if (cm->waittype == WAIT_TYPE_US)
				udelay(cm->wait);
			else if (cm->waittype == WAIT_TYPE_MS)
				mdelay(cm->wait);
			else
				mdelay(cm->wait * 1000);
		}
		cm++;
	}

	return cnt;
}

static void mipi_dsi_sread_request(struct dsi_cmd_desc *cm, char __iomem *dsi_base)
{
	u32 hdr = 0;

	/* fill up header */
	hdr |= DSI_HDR_DTYPE(cm->dtype);
	hdr |= DSI_HDR_VC(cm->vc);
	hdr |= DSI_HDR_DATA1(cm->payload[0]);
	hdr |= DSI_HDR_DATA2(0);
	set_reg(dsi_base + MIPIDSI_GEN_HDR_OFFSET, hdr, 24, 0);
}

static int mipi_dsi_read_add(u32 *out, struct dsi_cmd_desc *cm, char __iomem *dsi_base)
{
	int ret = 1;	//1: READ_SUCEES

	BUG_ON(cm == NULL);
	BUG_ON(dsi_base == NULL);

	if (cm->dtype == DTYPE_DCS_READ){
		mipi_dsi_sread_request(cm, dsi_base);

		if (cm->wait) {
			if (cm->waittype == WAIT_TYPE_US)
				udelay(cm->wait);
			else if (cm->waittype == WAIT_TYPE_MS)
				mdelay(cm->wait);
			else
				mdelay(cm->wait * 1000);
		}

		if (!mipi_dsi_read(out, dsi_base)) {
			ret = 0;
			HISI_FB_ERR("Read register 0x%X timeout\n",cm->payload[0]);
		}
	}
	else{
		ret = 0;
		HISI_FB_ERR("dtype=%x NOT supported!\n", cm->dtype);
	}

	return ret;
}

int mipi_dsi_cmds_rx(u32 *out, struct dsi_cmd_desc *cmds, int cnt,
	char __iomem *dsi_base)
{
	struct dsi_cmd_desc *cm = NULL;
	int i = 0;
	int err_num = 0;

	BUG_ON(cmds == NULL);
	BUG_ON(dsi_base == NULL);

	cm = cmds;

	for (i = 0; i < cnt; i++) {
		if(!mipi_dsi_read_add(&(out[i]), cm, dsi_base)){
			err_num++;
		}

		cm++;
	}
	return err_num;
}
