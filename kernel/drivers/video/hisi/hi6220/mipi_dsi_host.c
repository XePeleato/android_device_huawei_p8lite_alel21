/* Copyright (c) 2008-2011, Hisilicon Tech. Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *	 * Redistributions of source code must retain the above copyright
 *	   notice, this list of conditions and the following disclaimer.
 *	 * Redistributions in binary form must reproduce the above
 *	   copyright notice, this list of conditions and the following
 *	   disclaimer in the documentation and/or other materials provided
 *	   with the distribution.
 *	 * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *	   contributors may be used to endorse or promote products derived
 *	   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "mipi_dsi.h"
#include "mipi_reg.h"


/*
 * mipi dsi short write with 0, 1 2 parameters
 * Write to GEN_HDR 24 bit register the value:
 * 1. 00h, MCS_command[15:8] ,VC[7:6],13h
 * 2. Data1[23:16], MCS_command[15:8] ,VC[7:6],23h
 */
int mipi_dsi_swrite(struct dsi_cmd_desc *cm, u8 *dsi_base)
{
	u32 hdr = 0;
	int len = 0;

	if (cm->dlen && cm->payload == 0) {
		balongfb_loge("NO payload error!\n");
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

    set_MIPIDSI_GEN_HDR(dsi_base, hdr);

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
int mipi_dsi_lwrite(struct dsi_cmd_desc *cm, u8 *dsi_base)
{
	u32 hdr = 0;
	int i = 0;

	if (cm->dlen && cm->payload == 0) {
		balongfb_loge("NO payload error!\n");
		return 0;
	}

	/* fill up payload */
	for (i = 0;  i < cm->dlen; i += 4) {
        set_MIPIDSI_GEN_PLD_DATA(*((u32 *)(cm->payload + i)));
	}

	/* fill up header */
	hdr |= DSI_HDR_DTYPE(cm->dtype);
	hdr |= DSI_HDR_VC(cm->vc);
	hdr |= DSI_HDR_WC(cm->dlen);
    set_MIPIDSI_GEN_HDR(dsi_base, hdr);

	return cm->dlen;
}

void mipi_dsi_max_return_packet_size(struct dsi_cmd_desc *cm, u8 *dsi_base)
{
	u32 hdr = 0;

	/* fill up header */
	hdr |= DSI_HDR_DTYPE(cm->dtype);
	hdr |= DSI_HDR_VC(cm->vc);
	hdr |= DSI_HDR_WC(cm->dlen);
    set_MIPIDSI_GEN_HDR(dsi_base, hdr);
}

void mipi_dsi_sread(u32 *out, u8 *dsi_base)
{
	unsigned long dw_jiffies = 0;
    MIPIDSI_CMD_PKT_STATUS_UNION    mipidsi_cmd_pkt_status;


	dw_jiffies = jiffies + HZ / 2;
	do {
        mipidsi_cmd_pkt_status.ul32 = get_MIPIDSI_CMD_PKT_STATUS();
        if (0x1 == mipidsi_cmd_pkt_status.bits.gen_rd_cmd_busy) {
			break;
		}
	} while (time_after(dw_jiffies, jiffies));

	dw_jiffies = jiffies + HZ / 2;
	do {
        mipidsi_cmd_pkt_status.ul32 = get_MIPIDSI_CMD_PKT_STATUS();
        if (0x1 != mipidsi_cmd_pkt_status.bits.gen_rd_cmd_busy) {
			break;
		}
	} while (time_after(dw_jiffies, jiffies));

    *out = get_MIPIDSI_GEN_PLD_DATA();
}

void mipi_dsi_lread(u32 *out, u8 *dsi_base)
{
	/* do something here*/
}

/*
 * prepare cmd buffer to be txed
 */
int mipi_dsi_cmd_add(struct dsi_cmd_desc *cm, u8 *dsi_base)
{
	int len = 0;

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
		balongfb_loge("dtype=%x NOT supported!\n", cm->dtype);
		break;
	}

	return len;
}

int mipi_dsi_cmds_tx(struct dsi_cmd_desc *cmds, int cnt, u8 *dsi_base)
{
	struct dsi_cmd_desc *cm = NULL;
	int i = 0;

	BUG_ON(cmds == NULL);

	cm = cmds;

	for (i = 0; i < cnt; i++) {
        mipi_dsi_cmd_add(cm, dsi_base);

		if (cm->wait) {
			if (cm->waittype == WAIT_TYPE_US)
				udelay(cm->wait);
			else
				msleep(cm->wait);
		}
		cm++;
	}

	return cnt;
}
