/* Copyright (c) 2008-2013, Hisilicon Tech. Co., Ltd. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 * file: k3_mhl_tx.h
 */


#ifndef __K3_MHL_TX_H__
#define __K3_MHL_TX_H__

#include "k3_mhl_defs.h"

#define ENABLE_TX_DEBUG_PRINT 1

typedef struct
{
    u8 status_0;           // Received status from peer is stored here
    u8 status_1;           // Received status from peer is stored here

    u8 connected_ready;    // local MHL CONNECTED_RDY register value
    u8 link_mode;          // local MHL LINK_MODE register value

    bool tmds_output_enabled;

    bool  mhl_connection_event;
    u8 mhl_connected;
    u8 mhl_hpd_rsen_flags; // keep track of SET_HPD/CLR_HPD
    u8 rap_flags;          // CONTENT ON/OFF
    // Remember FEATURE FLAG of the peer to reject app commands if unsupported
    u8 msc_feature_flag;

    // msc_msg_arrived == true when a MSC MSG arrives, false when it has been picked up
    bool  msc_msg_arrived;
    u8 msc_msg_sub_command;
    u8 msc_msg_data;

    u8 cbus_reference_count;  // keep track of CBUS requests

    // Remember last command, offset that was sent.
    // Mostly for READ_DEVCAP command and other non-MSC_MSG commands
    u8 msc_last_command;
    u8 msc_last_offset;
    u8 msc_last_data;

    bool msc_cmd_in_progress;  //system is busy sending command.
    bool hdcp_start_pending;    //HDCP is waiting to begin. It was delayed because that system was busy sending command.

    // Remember last MSC_MSG command (RCPE particularly)
    u8 msc_msg_last_command;
    u8 msc_msg_last_data;
    u8 msc_save_rcp_key_code;

    bool disconnection_to_hdmi;
    bool connection_to_hdmi;
    bool real_disconnection;
#define MHL2_0_SUPPORT
#ifdef MHL2_0_SUPPORT
    u8      burst_entry_count3d_vic;
    u8      vic2d_index;
    u8      burst_entry_count3d_dtd;
    u8      vesa_dtd_index;
    u8      cea861_dtd_index;
#endif
    u32 misc_flags; //Rx scratchpad busy

    u8 uc_dev_cap_cache_index;

    mhl_dev_cap dev_cap_cache;

    u8 preferred_clk_mode;
} mhl_tx_config;

// bits for mhl_hpd_rsen_flags:
typedef enum
{
    MHL_HPD  = 0x01,
    MHL_RSEN = 0x02
} mhl_hpd_rsen;

typedef enum
{
    FLAGS_SCRATCHPAD_BUSY        = 0x0001,
    FLAGS_REQ_WRT_PENDING        = 0x0002,
    FLAGS_WRITE_BURST_PENDING    = 0x0004,
    FLAGS_RCP_READY              = 0x0008,
    FLAGS_HAVE_DEV_CATEGORY      = 0x0010,
    FLAGS_HAVE_DEV_FEATURE_FLAGS = 0x0020,
    FLAGS_HAVE_COMPLETE_DEVCAP   = 0x0040,
    FLAGS_SENT_DCAP_RDY          = 0x0080,
    FLAGS_SENT_PATH_EN           = 0x0100,
    FLAGS_SENT_3D_REQ            = 0x0200,
    FLAGS_BURST_3D_VIC_DONE      = 0x0400,
    FLAGS_BURST_3D_DTD_DONE      = 0x0800,
    FLAGS_BURST_3D_DTD_VESA_DONE = 0x1000,
    FLAGS_BURST_3D_DONE          = 0x2000,
    FLAGS_EDID_READ_DONE         = 0x4000
} MiscFlags_e;

typedef enum
{
    RAP_CONTENT_ON = 0x01
} rap_flags_e;


// structure to hold command details from upper layer to CBUS module
typedef struct
{
    u8 req_status;  // CBUS_IDLE, CBUS_PENDING
    u8 retry_count;
    u8 command;     // VS_CMD or RCP opcode
    u8 offset_data; // Offset of register on CBUS or RCP data
    u8 length;      // Only applicable to write burst. ignored otherwise.
    union
    {
        u8 msg_data[16];// Pointer to message data area.
        unsigned char *pdatabytes; // pointer for write burst or read many bytes
    }payload_u;
} cbus_req_t;

#define MHL_TX_EVENT_NONE               0x00    /* No event worth reporting.  */
#define MHL_TX_EVENT_DISCONNECTION      0x01    /* MHL connection has been lost */
#define MHL_TX_EVENT_CONNECTION         0x02    /* MHL connection has been established */
#define MHL_TX_EVENT_RCP_READY          0x03    /* MHL connection is ready for RCP */
#define MHL_TX_EVENT_RCP_RECEIVED       0x04    /* Received an RCP. Key Code in "eventParameter" */
#define MHL_TX_EVENT_RCPK_RECEIVED      0x05    /* Received an RCPK message */
#define MHL_TX_EVENT_RCPE_RECEIVED      0x06    /* Received an RCPE message .*/
#define MHL_TX_EVENT_DCAP_CHG           0x07    /* Received DCAP_CHG interrupt */
#define MHL_TX_EVENT_DSCR_CHG           0x08    /* Received DSCR_CHG interrupt */
#define MHL_TX_EVENT_POW_BIT_CHG        0x09    /* Peer's power capability has changed */
#define MHL_TX_EVENT_RGND_MHL           0x0A    /* RGND measurement has determine that the peer is an MHL device */

typedef enum {
    MHL_TX_EVENT_STATUS_HANDLED = 0,
    MHL_TX_EVENT_STATUS_PASSTHROUGH
} mhl_tx_notify_events_status;


#define MHL_STATUS_REG_CONNECTED_RDY        0x30
#define MHL_STATUS_REG_LINK_MODE            0x31

#define MHL_STATUS_DCAP_RDY                 BIT0

#define MHL_STATUS_CLK_MODE_MASK            0x07
#define MHL_STATUS_CLK_MODE_PACKED_PIXEL    0x02
#define MHL_STATUS_CLK_MODE_NORMAL          0x03
#define MHL_STATUS_PATH_EN_MASK             0x08
#define MHL_STATUS_PATH_ENABLED             0x08
#define MHL_STATUS_PATH_DISABLED            0x00
#define MHL_STATUS_MUTED_MASK               0x10


void k3_mhl_initialize(void);
void k3_mhl_process_events(void);
void k3_mhl_notify_connection(bool mhl_connected);
void k3_mhl_notify_hpd_change(u8 ds_hpd_status);
void k3_mhl_notify_rgnd_mhl(void);
void k3_mhl_got_msc_msg(u8 sub_command, u8 cmd_data);
void k3_mhl_got_intr(u8 intr_0, u8 intr_1);
void k3_mhl_got_status(u8 status_0, u8 status_1);
void k3_mhl_msc_command_done(u8 data1);
bool k3_mhl_rcp_send(u8 rcp_key_code);
bool k3_mhl_rcp_key_send(u8 rcp_key_code);
bool k3_mhl_rcp_error_send(u8 rcpe_error_code);
bool k3_mhl_set_path_en(void);
bool k3_mhl_clr_path_en(void);
bool k3_mhl_read_devcap(u8 offset);
u8   k3_mhl_get_peer_dev_cap_entry(u8 index, u8 *pdata);
mhl_tx_notify_events_status app_notify_mhl_event(u8 event_code, u8 event_param);

//#if USE_PP_MODE
void k3_mhl_tx_attempt_packed_pixel(void);
//#endif
void k3_mhl_reset_states(void);

#endif

