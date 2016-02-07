/* Copyright (c) 2008-2011, Hisilicon Tech. Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
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

#ifndef MIPI_DSI_H
#define MIPI_DSI_H

#include "balong_fb_def.h"


/* mipi dsi panel */
enum {
    DSI_VIDEO_MODE,
    DSI_CMD_MODE
};

enum {
    DSI_1_LANES = 0,
    DSI_2_LANES,
    DSI_3_LANES,
    DSI_4_LANES
};

enum {
    DSI_16BITS_1 = 0,
    DSI_16BITS_2,
    DSI_16BITS_3,
    DSI_18BITS_1,
    DSI_18BITS_2,
    DSI_24BITS_1,
    DSI_24BITS_2,
    DSI_24BITS_3
};

enum {
    DSI_NON_BURST_SYNC_PULSES = 0,
    DSI_NON_BURST_SYNC_EVENTS,
    DSI_BURST_SYNC_PULSES_1,
    DSI_BURST_SYNC_PULSES_2
};

#define DSI_VIDEO_DST_FORMAT_RGB565			0
#define DSI_VIDEO_DST_FORMAT_RGB666			1
#define DSI_VIDEO_DST_FORMAT_RGB666_LOOSE	2
#define DSI_VIDEO_DST_FORMAT_RGB888			3

#define DSI_CMD_DST_FORMAT_RGB565	0
#define DSI_CMD_DST_FORMAT_RGB666	1
#define DSI_CMD_DST_FORMAT_RGB888	2

/* dcs read/write */
#define DTYPE_DCS_WRITE		0x05	/* short write, 0 parameter */
#define DTYPE_DCS_WRITE1	0x15	/* short write, 1 parameter */
#define DTYPE_DCS_READ		0x06	/* read */
#define DTYPE_DCS_LWRITE	0x39	/* long write */

/* generic read/write */
#define DTYPE_GEN_WRITE		0x03	/* short write, 0 parameter */
#define DTYPE_GEN_WRITE1	0x13    /* short write, 1 parameter */
#define DTYPE_GEN_WRITE2	0x23	/* short write, 2 parameter */
#define DTYPE_GEN_LWRITE	0x29	/* long write */
#define DTYPE_GEN_READ		0x04	/* long read, 0 parameter */
#define DTYPE_GEN_READ1		0x14	/* long read, 1 parameter */
#define DTYPE_GEN_READ2		0x24	/* long read, 2 parameter */

#define DTYPE_TEAR_ON		0x35	/* set tear on */
#define DTYPE_MAX_PKTSIZE	0x37	/* set max packet size */
#define DTYPE_NULL_PKT		0x09	/* null packet, no data */
#define DTYPE_BLANK_PKT		0x19	/* blankiing packet, no data */

#define DTYPE_CM_ON				0x02	/* color mode off */
#define DTYPE_CM_OFF			0x12	/* color mode on */
#define DTYPE_PERIPHERAL_OFF	0x22
#define DTYPE_PERIPHERAL_ON		0x32

#define DSI_HDR_DTYPE(dtype)	((dtype) & 0x03f)
#define DSI_HDR_VC(vc)			(((vc) & 0x03) << 6)
#define DSI_HDR_DATA1(data)		(((data) & 0x0ff) << 8)
#define DSI_HDR_DATA2(data)		(((data) & 0x0ff) << 16)
#define DSI_HDR_WC(wc)			(((wc) & 0x0ffff) << 8)

#define DSI_PLD_DATA1(data)		((data) & 0x0ff)
#define DSI_PLD_DATA2(data)		(((data) & 0x0ff) << 8)
#define DSI_PLD_DATA3(data)		(((data) & 0x0ff) << 16)
#define DSI_PLD_DATA4(data)		(((data) & 0x0ff) << 24)

#define DEBUG_LCD_REG_PARA_MAX_NUM  (32)
#define DEBUG_LCD_CMD_STR_MAX_BYTE  (((DEBUG_LCD_REG_PARA_MAX_NUM + 2) * 11) + 4)
#define LCD_DEBUG_DIR_NAME           "lcd_debug"
#define LCD_DEBUG_FILE_NAME          "mipi_lcd_cmd"

enum {
    LCD_DEBUG_OPS_TYPE_WRITE = 0,
    LCD_DEBUG_OPS_TYPE_READ
};

struct dsi_cmd_desc {
    int dtype;
    int vc;
    int wait;
    int waittype;
    int dlen;
    char *payload;
};

/* used for debugfs to test LCD  */
struct lcd_debug_cmd {
    int cmd_type;           /* generic or dcs */
    int ops_type;           /* write or read */
    long lcd_reg_msb;        /* lcd reg high 8 bit */
    long lcd_reg_lsb;        /* lcd reg low 8 bit */
    long para_num;
    long para_array[DEBUG_LCD_REG_PARA_MAX_NUM];
};

/******************************************************************************
** FUNCTIONS PROTOTYPES
*/

void mipi_dsi_max_return_packet_size(struct dsi_cmd_desc *cm, u8 *dsi_base);
void mipi_dsi_sread(u32 *out, u8 *dsi_base);
void mipi_dsi_lread(u32 *out, u8 *dsi_base);
int mipi_dsi_swrite(struct dsi_cmd_desc *cm, u8 *dsi_base);
int mipi_dsi_lwrite(struct dsi_cmd_desc *cm, u8 *dsi_base);
int mipi_dsi_cmds_tx(struct dsi_cmd_desc *cmds, int cnt, u8 *dsi_base);


#endif  /* MIPI_DSI_H */
