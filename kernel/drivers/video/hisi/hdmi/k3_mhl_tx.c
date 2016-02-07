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

 * file: k3_mhl_tx.c
 */

#include <linux/kernel.h>
#include <linux/delay.h>
#include "k3_mhl_defs.h"
#include "k3_mhl_tx.h"
#include "k3_drv_mhl_tx.h"
#include "k3_hdmi.h"

#define LOG_TAG "mhl-tx"
#include "k3_hdmi_log.h"

#define MHL_DEV_LD_DISPLAY    (0x01 << 0)
#define MHL_DEV_LD_VIDEO      (0x01 << 1)
#define MHL_DEV_LD_AUDIO      (0x01 << 2)
#define MHL_DEV_LD_MEDIA      (0x01 << 3)
#define MHL_DEV_LD_TUNER      (0x01 << 4)
#define MHL_DEV_LD_RECORD     (0x01 << 5)
#define MHL_DEV_LD_SPEAKER    (0x01 << 6)
#define MHL_DEV_LD_GUI        (0x01 << 7)

#define MHL_MAX_RCP_KEY_CODE  (0x7F + 1)  // inclusive

extern hdmi_device hdmi;
extern u8 edid[HDMI_EDID_MAX_LENGTH];

//PLACE_IN_CODE_SEG u8 rcp_support_table [MHL_MAX_RCP_KEY_CODE] = {
u8 rcp_support_table [MHL_MAX_RCP_KEY_CODE] = {
    (MHL_DEV_LD_GUI),       // 0x00 = Select
    (MHL_DEV_LD_GUI),       // 0x01 = Up
    (MHL_DEV_LD_GUI),       // 0x02 = Down
    (MHL_DEV_LD_GUI),       // 0x03 = Left
    (MHL_DEV_LD_GUI),       // 0x04 = Right
    0, 0, 0, 0,             // 05-08 Reserved
    (MHL_DEV_LD_GUI),       // 0x09 = Root Menu
    0, 0, 0,                // 0A-0C Reserved
    (MHL_DEV_LD_GUI),       // 0x0D = Exit
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // 0E-1F Reserved
    (MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_TUNER),    // Numeric keys 0x20-0x29
    (MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_TUNER),
    (MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_TUNER),
    (MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_TUNER),
    (MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_TUNER),
    (MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_TUNER),
    (MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_TUNER),
    (MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_TUNER),
    (MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_TUNER),
    (MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_TUNER),
    0,                      // 0x2A = Dot
    (MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_TUNER),    // Enter key = 0x2B
    (MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_TUNER),    // Clear key = 0x2C
    0, 0, 0,                // 2D-2F Reserved
    (MHL_DEV_LD_TUNER),     // 0x30 = Channel Up
    (MHL_DEV_LD_TUNER),     // 0x31 = Channel Dn
    (MHL_DEV_LD_TUNER),     // 0x32 = Previous Channel
    (MHL_DEV_LD_AUDIO),     // 0x33 = Sound Select
    0,                      // 0x34 = Input Select
    0,                      // 0x35 = Show Information
    0,                      // 0x36 = Help
    0,                      // 0x37 = Page Up
    0,                      // 0x38 = Page Down
    0, 0, 0, 0, 0, 0, 0,    // 0x39-0x3F Reserved
    0,                      // 0x40 = Undefined

    (MHL_DEV_LD_SPEAKER),   // 0x41 = Volume Up
    (MHL_DEV_LD_SPEAKER),   // 0x42 = Volume Down
    (MHL_DEV_LD_SPEAKER),   // 0x43 = Mute
    (MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO),  // 0x44 = Play
    (MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_RECORD),  // 0x45 = Stop
    (MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_RECORD),  // 0x46 = Pause
    (MHL_DEV_LD_RECORD),    // 0x47 = Record
    (MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO),  // 0x48 = Rewind
    (MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO),  // 0x49 = Fast Forward
    (MHL_DEV_LD_MEDIA),     // 0x4A = Eject
    (MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA),   // 0x4B = Forward
    (MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA),   // 0x4C = Backward
    0, 0, 0,                // 4D-4F Reserved
    0,                      // 0x50 = Angle
    0,                      // 0x51 = Subpicture
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 52-5F Reserved
    (MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO),  // 0x60 = Play Function
    (MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO),  // 0x61 = Pause the Play Function
    (MHL_DEV_LD_RECORD),    // 0x62 = Record Function
    (MHL_DEV_LD_RECORD),    // 0x63 = Pause the Record Function
    (MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_RECORD),  // 0x64 = Stop Function

    (MHL_DEV_LD_SPEAKER),   // 0x65 = Mute Function
    (MHL_DEV_LD_SPEAKER),   // 0x66 = Restore Mute Function
    0, 0, 0, 0, 0, 0, 0, 0, 0,                          // 0x67-0x6F Undefined or reserved
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0      // 0x70-0x7F Undefined or reserved
};

unsigned short rcp_key_map_table[MHL_MAX_RCP_KEY_CODE] =
{
    KEY_REPLY, //MHL_RCP_CMD_SELECT
    KEY_UP, //MHL_RCP_CMD_UP
    KEY_DOWN, //MHL_RCP_CMD_DOWN
    KEY_LEFT, //MHL_RCP_CMD_LEFT
    KEY_RIGHT, //MHL_RCP_CMD_RIGHT
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_HOMEPAGE,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_BACK, //=0X0D, MHL_RCP_CMD_EXIT

    /* 0X0E - 0X1F are reserved */
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,

    KEY_0, //=0x20, MHL_RCP_CMD_NUM_0
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9, //=0x29, MHL_RCP_CMD_NUM_9

    KEY_DOT, //=0x2A, MHL_RCP_CMD_DOT
    KEY_ENTER,
    KEY_BACKSPACE, //=0x2C, MHL_RCP_CMD_CLEAR

    /* 0X2D - 0X2F are reserved */
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,

    KEY_CHANNELUP,
    KEY_CHANNELDOWN,
    KEY_PREVIOUSSONG,
    KEY_SOUND,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,

    /* 0X39 - 0X40 are reserved */
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,

    KEY_VOLUMEUP,
    KEY_VOLUMEDOWN,
    KEY_MUTE,
    KEY_PLAY,
    KEY_STOP,
    KEY_PLAYPAUSE,
    KEY_RECORD,
    KEY_REWIND,
    KEY_FASTFORWARD,
    KEY_EJECTCD,
    KEY_FORWARD,
    KEY_PREVIOUSSONG,

    /* 0X4D - 0X4F are reserved */
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,

    KEY_RESERVED,
    KEY_RESERVED,

    /* 0X52 - 0X5F are reserved */
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,

    KEY_PLAYCD,
    KEY_PAUSECD,
    KEY_RECORD,
    KEY_RESERVED,
    KEY_STOP,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,

    /* 0X69 - 0X70 are reserved */
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,

    KEY_F1,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,

    /* 0X76 - 0X7D are reserved */
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,
    KEY_RESERVED,

    KEY_RESERVED,
    KEY_RESERVED
};

/*queue implementation*/
#define NUM_CBUS_EVENT_QUEUE_EVENTS 5
typedef struct _CBus_queue_t
{
    u8 head; // queue empty condition head == tail
    u8 tail;
    cbus_req_t queue[NUM_CBUS_EVENT_QUEUE_EVENTS];
} CBus_queue_t, *pCBus_queue_t;

#define QUEUE_SIZE(x) (sizeof(x.queue) / sizeof(x.queue[0]))
#define MAX_QUEUE_DEPTH(x) (QUEUE_SIZE(x) - 1)
#define QUEUE_DEPTH(x) ((x.head <= x.tail) ? (x.tail - x.head) : (QUEUE_SIZE(x) - x.head + x.tail))
#define QUEUE_FULL(x) (QUEUE_DEPTH(x) >= MAX_QUEUE_DEPTH(x))

#define ADVANCE_QUEUE_HEAD(x) { x.head = (x.head < MAX_QUEUE_DEPTH(x)) ? (x.head + 1) : 0; }
#define ADVANCE_QUEUE_TAIL(x) { x.tail = (x.tail < MAX_QUEUE_DEPTH(x)) ? (x.tail + 1) : 0; }

#define RETREAT_QUEUE_HEAD(x) { x.head = (x.head > 0) ? (x.head - 1) : MAX_QUEUE_DEPTH(x); }

CBus_queue_t CBus_queue;

cbus_req_t *get_next_cbus_transaction_impl(void)
{
    if (0 == QUEUE_DEPTH(CBus_queue)){
        return NULL;
    } else {
        cbus_req_t *retVal;
        retVal = &CBus_queue.queue[CBus_queue.head];
        ADVANCE_QUEUE_HEAD(CBus_queue)
        return retVal;
    }
}

#ifdef ENABLE_TX_DEBUG_PRINT
cbus_req_t *get_next_cbus_transaction_wrapper(char *pszFunction)
{
    logd("GET_NEXT_CBUS_TRANSACTION: %s depth: %d  head: %d  tail: %d\n",
        pszFunction, (int)QUEUE_DEPTH(CBus_queue), (int)CBus_queue.head,(int)CBus_queue.tail);
    mdelay(5);
    return get_next_cbus_transaction_impl();
}

#define GET_NEXT_CBUS_TRANSACTION(func) get_next_cbus_transaction_wrapper(# func)
#else
#define GET_NEXT_CBUS_TRANSACTION(func) get_next_cbus_transaction_impl()
#endif

bool put_next_cbus_transaction_impl(cbus_req_t *p_req)
{
    if (QUEUE_FULL(CBus_queue)) {
        //queue is full
        return false;
    }

    // at least one slot available
    CBus_queue.queue[CBus_queue.tail] = *p_req;
    ADVANCE_QUEUE_TAIL(CBus_queue);
    return true;
}

#ifdef ENABLE_TX_DEBUG_PRINT
//use this wrapper to do debugging output for the routine above.
bool put_next_cbus_transaction_wrapper(cbus_req_t *p_req)
{
    bool ret_val;
    logd("PUT_NEXT_CBUS_TRANSACTION 0x%02X 0x%02X 0x%02X depth:%d head: %d tail:%d\n",
        (int)p_req->command,
        (int)((MHL_MSC_MSG == p_req->command) ? p_req->payload_u.msg_data[0] : p_req->offset_data),
        (int)((MHL_MSC_MSG == p_req->command) ? p_req->payload_u.msg_data[1] : p_req->payload_u.msg_data[0]),
        (int)QUEUE_DEPTH(CBus_queue), (int)CBus_queue.head, (int)CBus_queue.tail);

    ret_val = put_next_cbus_transaction_impl(p_req);
    if (!ret_val) {
        loge("PUT_NEXT_CBUS_TRANSACTION queue full, when adding event 0x%02x\n",
            (int)p_req->command);
    }
    return ret_val;
}
#define PUT_NEXT_CBUS_TRANSACTION(req) put_next_cbus_transaction_wrapper(req)
#else
#define PUT_NEXT_CBUS_TRANSACTION(req) put_next_cbus_transaction_impl(req)
#endif

bool put_priority_cbus_transaction_impl(cbus_req_t *p_req)
{
    if (QUEUE_FULL(CBus_queue)) {
        //queue is full
        loge("Queque is full\n");
        return false;
    }

    // at least one slot available
    RETREAT_QUEUE_HEAD(CBus_queue);
    CBus_queue.queue[CBus_queue.head] = *p_req;
    return true;
}

#ifdef ENABLE_TX_DEBUG_PRINT
bool put_priority_cbus_transaction_wrapper(cbus_req_t *p_req)
{
    bool ret_val;
    logd("PUT_PRIORITY_CBUS_TRANSACTION 0x%02X 0x%02X 0x%02X depth:%d head: %d tail:%d\n",
        (int)p_req->command,
        (int)((MHL_MSC_MSG == p_req->command) ? p_req->payload_u.msg_data[0] : p_req->offset_data),
        (int)((MHL_MSC_MSG == p_req->command) ? p_req->payload_u.msg_data[1] : p_req->payload_u.msg_data[0]),
        (int)QUEUE_DEPTH(CBus_queue),
        (int)CBus_queue.head,
        (int)CBus_queue.tail);
    ret_val = put_priority_cbus_transaction_impl(p_req);
    if (!ret_val) {
        loge("PUT_PRIORITY_CBUS_TRANSACTION queue full, when adding event 0x%02X\n",
                         (int)p_req->command);
    }
    return ret_val;
}

#define PUT_PRIORITY_CBUS_TRANSACTION(p_req) put_priority_cbus_transaction_wrapper(p_req)
#else
#define PUT_PRIORITY_CBUS_TRANSACTION(p_req) put_priority_cbus_transaction_impl(p_req)
#endif

mhl_tx_config g_mhl_tx_config = {0};

void k3_mhl_reset_states(void);
static bool k3_mhl_set_status(u8 regToWrite, u8 value);
static bool k3_mhl_send_link_mode(void);
static void k3_mhl_tx_drive_states(void);
static bool k3_mhl_set_DCap_rdy(void);
static bool k3_mhl_rapk_send(u8 rapkErrCode);
static void k3_mhl_refresh_peer_devCap_entries(void);
static bool k3_mhl_tx_send_msc_msg(u8 command, u8 cmd_data);
static bool k3_mhl_set_int(u8 regToWrite, u8 mask, u8 priorityLevel);

/******************************************************************************
* Function:     k3_mhl_qualify_path_enable
* Description:  Support MHL 1.0 sink devices.
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:       1 - consider PATH_EN received
                0 - consider PATH_EN NOT received
* Others:
*******************************************************************************/
static u8 k3_mhl_qualify_path_enable(void)
{
    u8 ret_val = 0; //return fail by default

    IN_FUNCTION;

    if (MHL_STATUS_PATH_ENABLED & g_mhl_tx_config.status_1) {
        logd("\t\t\tMHL_STATUS_PATH_ENABLED\n");
        ret_val = 1;
    } else {
        if (0x10 == g_mhl_tx_config.dev_cap_cache.auc_dev_cap_cache[DEVCAP_OFFSET_MHL_VERSION]
            && 0x44 == g_mhl_tx_config.dev_cap_cache.auc_dev_cap_cache[DEVCAP_OFFSET_INT_STAT_SIZE]) {
            logd("\t\t\tLegacy Support for MHL_STATUS_PATH_ENABLED\n");
            ret_val = 1;
        }
    }

    OUT_FUNCTION;
    return ret_val;
}

/********************************************************************************
* Function:       k3_mhl_tmds_enable
* Description:    Implements conditions on enabling TMDS output stated in MHL spec section 7.6.1
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
static void k3_mhl_tmds_enable(void)
{
    IN_FUNCTION;

    if (0 == (MHL_RSEN & g_mhl_tx_config.mhl_hpd_rsen_flags)) {
        OUT_FUNCTION;
        return;
    }

    logi("\tMHL_RSEN\n");

    if (0 == (MHL_HPD & g_mhl_tx_config.mhl_hpd_rsen_flags)) {
        OUT_FUNCTION;
        return;
    }

    logi("\tMHL_HPD\n");

    if (k3_mhl_qualify_path_enable()
        && (RAP_CONTENT_ON & g_mhl_tx_config.rap_flags)) {
        logi("\t\t\t\tRAP CONTENT_ON\n");
        k3_mhl_tmds_control(true);
    }

    OUT_FUNCTION;
}

/********************************************************************************
* Function:       k3_mhl_set_int
* Description:    Set MHL defined INTERRUPT bits in peer's register set.
* Data Accessed:
* Data Updated:
* Input:          regToWrite     Remote interrupt register to write
*                 mask              the bits to write to that register
*                 priority           0:  add to head of CBus_queue
*                                        1:  add to tail of CBus_queue
* Output:
* Return:         true: if operation was successfully performed.
* Others:
*********************************************************************************/
static bool k3_mhl_set_int(u8 regToWrite, u8 mask, u8 priorityLevel)
{
    cbus_req_t req;
    bool ret_val;

    IN_FUNCTION;

    // find the offset and bit position
    // and feed
    req.retry_count = 2;
    req.command = MHL_SET_INT;
    req.offset_data = regToWrite;
    req.payload_u.msg_data[0] = mask;
    if (0 == priorityLevel) {
        ret_val = PUT_PRIORITY_CBUS_TRANSACTION(&req);
    } else {
        ret_val = PUT_NEXT_CBUS_TRANSACTION(&req);
    }

    OUT_FUNCTION;
    return ret_val;
}

/********************************************************************************
* Function:       k3_mhl_initialize
* Description:    init mhl
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
void k3_mhl_initialize(void)
{
    // Initialize queue of pending CBUS requests.
    CBus_queue.head = 0;
    CBus_queue.tail = 0;

    IN_FUNCTION;

    k3_mhl_reset_states();
    k3_mhl_chip_initialize();

    OUT_FUNCTION;
}

/********************************************************************************
* Function:       k3_mhl_reset_states
* Description:    Application picks up mhl connection and rcp events at periodic intervals.
*                 Interrupt handler feeds these variables. Reset them on disconnection.
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
void k3_mhl_reset_states(void)
{
    IN_FUNCTION;

    g_mhl_tx_config.mhl_connection_event = false;
    g_mhl_tx_config.mhl_connected = MHL_TX_EVENT_DISCONNECTION;
    g_mhl_tx_config.mhl_hpd_rsen_flags &= ~(MHL_RSEN | MHL_HPD);
    g_mhl_tx_config.rap_flags &= ~RAP_CONTENT_ON;
    g_mhl_tx_config.tmds_output_enabled = false;

    g_mhl_tx_config.msc_msg_arrived = false;
    g_mhl_tx_config.msc_msg_sub_command = 0;

    g_mhl_tx_config.status_0 = 0;
    g_mhl_tx_config.status_1 = 0;
    g_mhl_tx_config.connected_ready = 0;
    g_mhl_tx_config.link_mode = MHL_STATUS_CLK_MODE_NORMAL; // indicate normal (24-bit) mode
    g_mhl_tx_config.preferred_clk_mode   = MHL_STATUS_CLK_MODE_NORMAL;// this can be overridden by the application calling SetPreferredPixelFormat()
    g_mhl_tx_config.cbus_reference_count = 0;
    g_mhl_tx_config.misc_flags = 0;
    g_mhl_tx_config.msc_last_command = 0;
    g_mhl_tx_config.msc_msg_last_command = 0;
    g_mhl_tx_config.msc_last_data = 0;
    g_mhl_tx_config.dev_cap_cache.uc_dev_cap_cache_index = 1 + sizeof(g_mhl_tx_config.dev_cap_cache.auc_dev_cap_cache);

    g_mhl_tx_config.disconnection_to_hdmi = false;
    g_mhl_tx_config.connection_to_hdmi = false;

    OUT_FUNCTION;
}

/********************************************************************************
* Function:       k3_mhl_set_status
* Description:    Set MHL defined STATUS bits in peer's register set.
* Data Accessed:
* Data Updated:
* Input:          regToWrite : MHLRegister to write
*                 value : data to write to the register
* Output:
* Return:         bool
* Others:
*********************************************************************************/
static bool k3_mhl_set_status(u8 regToWrite, u8 value)
{
    cbus_req_t req;
    bool ret_val = false;

    IN_FUNCTION;
    // find the offset and bit position
    // and feed
    req.retry_count = 2;
    req.command = MHL_WRITE_STAT;
    req.offset_data = regToWrite;
    req.payload_u.msg_data[0] = value;
    ret_val = PUT_NEXT_CBUS_TRANSACTION(&req);

    OUT_FUNCTION;
    return ret_val;
}

/********************************************************************************
* Function:       k3_mhl_send_link_mode
* Description:
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
static bool k3_mhl_send_link_mode(void)
{
    bool ret;

    IN_FUNCTION;

    logd("link_mode:0x%x\n", g_mhl_tx_config.link_mode);
    ret = k3_mhl_set_status(MHL_STATUS_REG_LINK_MODE, g_mhl_tx_config.link_mode);

    OUT_FUNCTION;
    return ret;
}

/********************************************************************************
* Function:       k3_mhl_process_connection_event
* Description:    process events when connection or disconnection
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
void k3_mhl_process_connection_event(void)
{
    IN_FUNCTION;

    /* Consume the message */
    g_mhl_tx_config.mhl_connection_event = false;
    /* Let app know about the change of the connection state */
    app_notify_mhl_event(g_mhl_tx_config.mhl_connected, g_mhl_tx_config.msc_feature_flag);

    if (MHL_TX_EVENT_DISCONNECTION == g_mhl_tx_config.mhl_connected) {
        /* If connection has been lost, reset all state flags */
        k3_mhl_reset_states();
    } else if (MHL_TX_EVENT_CONNECTION == g_mhl_tx_config.mhl_connected) {
        k3_mhl_set_DCap_rdy();
    }

    OUT_FUNCTION;
}

/********************************************************************************
* Function:       k3_mhl_process_receive_rap_event
* Description:    process events when receive rap
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
void k3_mhl_process_receive_rap_event(void)
{
    IN_FUNCTION;

    switch (g_mhl_tx_config.msc_msg_data)
    {
        case MHL_RAP_CONTENT_ON : {
            g_mhl_tx_config.rap_flags |= RAP_CONTENT_ON;
            logi("RAP CONTENT_ON\n");
            k3_mhl_tmds_enable();
            k3_mhl_rapk_send(MHL_MSC_MSG_RAP_NO_ERROR);
            break;
        }
        case MHL_RAP_CONTENT_OFF : {
            g_mhl_tx_config.rap_flags &= ~RAP_CONTENT_ON;
            logi("RAP CONTENT_OFF\n");
            k3_mhl_tmds_control( false );
            k3_mhl_rapk_send(MHL_MSC_MSG_RAP_NO_ERROR);
            break;
        }
        case MHL_RAP_CMD_POLL : {
            k3_mhl_rapk_send(MHL_MSC_MSG_RAP_UNSUPPORTED_ACT_CODE);
            break;
        }
        default : {
            k3_mhl_rapk_send(MHL_MSC_MSG_RAP_UNRECOGNIZED_ACT_CODE);
            break;
        }
    }

    OUT_FUNCTION;
}

/********************************************************************************
* Function:       k3_mhl_process_receive_rcp_event
* Description:    process events when receive rcp
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
void k3_mhl_process_receive_rcp_event(void)
{
    IN_FUNCTION;

    if (MHL_LOGICAL_DEVICE_MAP & rcp_support_table[g_mhl_tx_config.msc_msg_data & 0x7F]) {
        app_notify_mhl_event(MHL_TX_EVENT_RCP_RECEIVED, g_mhl_tx_config.msc_msg_data);
    } else {
        /* If we get a RCP key that we do NOT support, Save keycode to send
           a RCPK after RCPE,do not notify app layer */
        g_mhl_tx_config.msc_save_rcp_key_code = g_mhl_tx_config.msc_msg_data;
        k3_mhl_rcp_error_send(RCPE_INEEFECTIVE_KEY_CODE);
    }

    OUT_FUNCTION;
}


/********************************************************************************
* Function:       k3_mhl_process_receive_rcpk_event
* Description:    process events when receive rcpk
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
void k3_mhl_process_receive_rcpk_event(void)
{
    IN_FUNCTION;

    app_notify_mhl_event(MHL_TX_EVENT_RCPK_RECEIVED, g_mhl_tx_config.msc_msg_data);
    DECREMENT_CBUS_REFERENCE_COUNT(k3_mhl_process_events);
    g_mhl_tx_config.msc_last_command = 0;
    g_mhl_tx_config.msc_msg_last_command = 0;

    OUT_FUNCTION;
}

/********************************************************************************
* Function:       k3_mhl_process_receive_rcpe_event
* Description:    process events when receive rcpe
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
void k3_mhl_process_receive_rcpe_event(void)
{
    IN_FUNCTION;

    app_notify_mhl_event(MHL_TX_EVENT_RCPE_RECEIVED, g_mhl_tx_config.msc_msg_data);

    OUT_FUNCTION;
}

/********************************************************************************
* Function:       k3_mhl_process_receive_rapk_event
* Description:    process events when receive rapk
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
void k3_mhl_process_receive_rapk_event(void)
{
    IN_FUNCTION;

    // Do nothing if RAPK comes, except decrement the reference counter
    DECREMENT_CBUS_REFERENCE_COUNT(k3_mhl_process_events);
    g_mhl_tx_config.msc_last_command = 0;
    g_mhl_tx_config.msc_msg_last_command = 0;

    OUT_FUNCTION;
}

/********************************************************************************
* Function:       k3_mhl_process_events
* Description:    This internal function is called at the end of interrupt processing.
*                 It's purpose is to process events detected during the interrupt.
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
void k3_mhl_process_events(void)
{
    IN_FUNCTION;

    // Make sure any events detected during the interrupt are processed.
    k3_mhl_tx_drive_states();

    if (g_mhl_tx_config.mhl_connection_event) {
        k3_mhl_process_connection_event();
    } else if (g_mhl_tx_config.msc_msg_arrived) {
        logi("MSC MSG <0x%02x, 0x%02x>\n",
            g_mhl_tx_config.msc_msg_sub_command,
            g_mhl_tx_config.msc_msg_data);

        // Consume the message
        g_mhl_tx_config.msc_msg_arrived = false;

        // Map sub-command to an event id
        switch (g_mhl_tx_config.msc_msg_sub_command) {
            case MHL_MSC_MSG_RAP:
                /* RAP is fully handled here */
                k3_mhl_process_receive_rap_event();
                break;
            case MHL_MSC_MSG_RCP:
                k3_mhl_process_receive_rcp_event();
                break;
            case MHL_MSC_MSG_RCPK:
                k3_mhl_process_receive_rcpk_event();
                break;
            case MHL_MSC_MSG_RCPE:
                k3_mhl_process_receive_rcpe_event();
                break;
            case MHL_MSC_MSG_RAPK:
                k3_mhl_process_receive_rapk_event();
                break;
            default:
                // Any freak value here would continue with no event to app
                break;
        }
    }

    OUT_FUNCTION;
}

/********************************************************************************
* Function:       k3_mhl_tx_drive_states
* Description:    This function is called by the interrupt handler in the driver layer to move the
*                 MSC engine to do the next thing before allowing the application to run RCP APIs.
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
static void k3_mhl_tx_drive_states(void)
{
    static u8 fail_count = 0;
    u8 re_queue_request = 0;
    cbus_req_t *req = NULL;

    IN_FUNCTION;

    /* process queued CBus transactions */
    if (QUEUE_DEPTH(CBus_queue) <= 0) {
        OUT_FUNCTION;
        return;
    }

    logd("CBus_queue is not empty!\n");

    if (k3_mhl_cbus_busy()) {
        loge("CBusBusy!\n");
        fail_count++;

        /* if Send one Command more than 50 times, cancel sending */
        if (fail_count > 5) {
            loge("Cancel sending.\n");
            g_mhl_tx_config.msc_cmd_in_progress = false;
            DECREMENT_CBUS_REFERENCE_COUNT(k3_mhl_tx_drive_states);
        }
        OUT_FUNCTION;
        return;
    }

    /* QUEUE is not empty and cbus is not busy, process */
    fail_count = 0;
    req = GET_NEXT_CBUS_TRANSACTION(k3_mhl_tx_drive_states);

    // coordinate write burst requests and grants.
    if (MHL_SET_INT == req->command
        && MHL_RCHANGE_INT == req->offset_data) {
        if (FLAGS_SCRATCHPAD_BUSY & g_mhl_tx_config.misc_flags) {
            if (MHL_INT_REQ_WRT == req->payload_u.msg_data[0]) {
                re_queue_request = 1;
            }
        } else {
            if (MHL_INT_REQ_WRT == req->payload_u.msg_data[0]) {
                INCREMENT_CBUS_REFERENCE_COUNT(k3_mhl_tx_drive_states)
                SET_MISC_FLAG(k3_mhl_tx_drive_states, FLAGS_SCRATCHPAD_BUSY)
                SET_MISC_FLAG(k3_mhl_tx_drive_states, FLAGS_WRITE_BURST_PENDING)
            } else if (MHL_INT_GRT_WRT == req->payload_u.msg_data[0]) {
                SET_MISC_FLAG(k3_mhl_tx_drive_states, FLAGS_SCRATCHPAD_BUSY)
            }
        }

    }

    if (re_queue_request) {
        /* send this one to the back of the line for later attempts */
        if (req->retry_count-- > 0) {
            PUT_NEXT_CBUS_TRANSACTION(req);
        }
        OUT_FUNCTION;
        return;
    }

    if (MHL_MSC_MSG == req->command) {
        g_mhl_tx_config.msc_msg_last_command = req->payload_u.msg_data[0];
        g_mhl_tx_config.msc_msg_last_data = req->payload_u.msg_data[1];
    } else {
        g_mhl_tx_config.msc_last_offset = req->offset_data;
        g_mhl_tx_config.msc_last_data = req->payload_u.msg_data[0];
    }

    g_mhl_tx_config.msc_last_command = req->command;

    INCREMENT_CBUS_REFERENCE_COUNT(k3_mhl_tx_drive_states)
    k3_mhl_send_cbus_command(req);

    OUT_FUNCTION;
}

/********************************************************************************
* Function:      k3_mhl_msc_command_read_devcap_done
* Description:
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
void k3_mhl_msc_command_read_devcap_done(u8 data1)
{
    u8 param;
    u8 hpd_status = 0;

    IN_FUNCTION;

    if (g_mhl_tx_config.msc_last_offset >= DEVCAP_SIZE) {
        OUT_FUNCTION;
        return;
    }

    // populate the cache
    g_mhl_tx_config.dev_cap_cache.auc_dev_cap_cache[g_mhl_tx_config.msc_last_offset] = data1;
    logd("peer DEV_CAP[0x%02x]:0x%02x index:0x%02x\n",
        g_mhl_tx_config.msc_last_offset, data1,
        g_mhl_tx_config.dev_cap_cache.uc_dev_cap_cache_index);

    switch (g_mhl_tx_config.msc_last_offset) {
        case MHL_DEV_CATEGORY_OFFSET : {
            logd("MhlTx: FLAGS_HAVE_DEV_CATEGORY\n");
            g_mhl_tx_config.misc_flags |= FLAGS_HAVE_DEV_CATEGORY;

            /* Give the OEM a chance at handling power for himself */
            param = data1 & MHL_DEV_CATEGORY_POW_BIT;
            if (MHL_TX_EVENT_STATUS_PASSTHROUGH ==
                app_notify_mhl_event(MHL_TX_EVENT_POW_BIT_CHG, param)) {
                k3_mhl_pow_bit_change((bool)param);
            }
            break;
        }

        case MHL_DEV_FEATURE_FLAG_OFFSET : {
            logd("MhlTx: FLAGS_HAVE_DEV_FEATURE_FLAGS\n");
            g_mhl_tx_config.misc_flags |= FLAGS_HAVE_DEV_FEATURE_FLAGS;

            /* Remember features of the peer*/
            g_mhl_tx_config.msc_feature_flag = data1;
            logd("MhlTx: Peer's Feature Flag = 0x%02x\n", data1);
            break;
        }

        case DEVCAP_OFFSET_VID_LINK_MODE : {
            logd("to do--call examine_local_and_peer_vid_link_mode\n");
            //examine_local_and_peer_vid_link_mode();//to do when ppmode
            break;
        }

        default :
            break;
    }

    g_mhl_tx_config.dev_cap_cache.uc_dev_cap_cache_index++;

    if (g_mhl_tx_config.dev_cap_cache.uc_dev_cap_cache_index
        < DEVCAP_SIZE) {
        /* OK to call this here, since requests always get queued
           and processed in the "foreground */
        k3_mhl_read_devcap(g_mhl_tx_config.dev_cap_cache.uc_dev_cap_cache_index);
    } else {
        /* this is necessary for both firmware and linux driver */
        app_notify_mhl_event(MHL_TX_EVENT_DCAP_CHG, 0);

        hpd_status = MHL_HPD & g_mhl_tx_config.mhl_hpd_rsen_flags;
        logd("HPD status: %d\n", hpd_status);

        // These variables are used to remember if we issued a READ_DEVCAP
        // or other MSC command.Since we are done, reset them.
        g_mhl_tx_config.msc_last_command = 0;
        g_mhl_tx_config.msc_last_offset = 0;
    }

    OUT_FUNCTION;
}

/********************************************************************************
* Function:      k3_mhl_msc_command_write_stat_done
* Description:
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
void k3_mhl_msc_command_write_stat_done(void)
{
    IN_FUNCTION;

    logd("misc_flags: 0x%02x\n", g_mhl_tx_config.misc_flags);

    if (MHL_STATUS_REG_CONNECTED_RDY == g_mhl_tx_config.msc_last_offset) {
        if (MHL_STATUS_DCAP_RDY & g_mhl_tx_config.msc_last_data) {
            logd("FLAGS_SENT_DCAP_RDY\n");
            g_mhl_tx_config.misc_flags |= FLAGS_SENT_DCAP_RDY;

            k3_mhl_set_int(MHL_RCHANGE_INT, MHL_INT_DCAP_CHG, 0);// priority request
        }
    } else if (MHL_STATUS_REG_LINK_MODE == g_mhl_tx_config.msc_last_offset) {
        if (MHL_STATUS_PATH_ENABLED & g_mhl_tx_config.msc_last_data) {
            logd("FLAGS_SENT_PATH_EN\n");
            g_mhl_tx_config.misc_flags |= FLAGS_SENT_PATH_EN;
        }
    }

    g_mhl_tx_config.msc_last_command = 0;
    g_mhl_tx_config.msc_last_offset = 0;

    OUT_FUNCTION;
}

/********************************************************************************
* Function:      k3_mhl_msc_command_msc_msg_done
* Description:
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
void k3_mhl_msc_command_msc_msg_done(void)
{
    IN_FUNCTION;

    if (MHL_MSC_MSG_RCPE == g_mhl_tx_config.msc_msg_last_command) {
        /* RCPE is always followed by an RCPK with original key code that came. */
        k3_mhl_rcp_key_send(g_mhl_tx_config.msc_save_rcp_key_code);
    } else {
        logd("default\n"
            "\tmscLastCommand: 0x%02x \n"
            "\tmscMsgLastCommand: 0x%02x msc_msg_last_data: 0x%02x\n"
            "\tcbusReferenceCount: %d\n",
            g_mhl_tx_config.msc_last_command,
            g_mhl_tx_config.msc_msg_last_command,
            g_mhl_tx_config.msc_msg_last_data,
            g_mhl_tx_config.cbus_reference_count);
    }

    g_mhl_tx_config.msc_last_command = 0;

    OUT_FUNCTION;
}

/********************************************************************************
* Function:      k3_mhl_msc_command_write_burst_done
* Description:
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
void k3_mhl_msc_command_write_burst_done(void)
{
    IN_FUNCTION;

    g_mhl_tx_config.msc_last_command = 0;
    g_mhl_tx_config.msc_last_offset = 0;
    g_mhl_tx_config.msc_last_data = 0;

    /* all CBus request are queued, so this is OK to call here,
       use priority 0 so that other queued commands don't interfere */
    k3_mhl_set_int(MHL_RCHANGE_INT, MHL_INT_DSCR_CHG, 0);

    OUT_FUNCTION;
}

/********************************************************************************
* Function:      k3_mhl_msc_command_set_int_done
* Description:
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
void k3_mhl_msc_command_set_int_done(void)
{
    IN_FUNCTION;

    if (MHL_RCHANGE_INT == g_mhl_tx_config.msc_last_offset) {
        logd("MhlTx: MHL_RCHANGE_INT: MHL_INT_DSCR_CHG(0x02)"
            "MHL_INT_REQ_WRT(0x04) MHL_INT_GRT_WRT(0x08) MHL2_INT_3D_REQ(0x10)\n");
        logd("MhlTx: 0x%02x sent successfully.\n", g_mhl_tx_config.msc_last_data);
    }

    // Once the command has been sent out successfully, forget this case.
    g_mhl_tx_config.msc_last_command = 0;
    g_mhl_tx_config.msc_last_offset = 0;
    g_mhl_tx_config.msc_last_data = 0;

    OUT_FUNCTION;
}


/********************************************************************************
* Function:      k3_mhl_msc_command_done
* Description:   This function is called by the driver to inform of completion of last command.
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:        It is called in interrupt context to meet some MHL specified timings, therefore, it should
*                not have to call app layer and do negligible processing, no printfs.
*********************************************************************************/
#define FLAG_OR_NOT(x) (FLAGS_HAVE_ ## x & g_mhl_tx_config.misc_flags) ? # x : ""
#define SENT_OR_NOT(x) (FLAGS_SENT_ ## x & g_mhl_tx_config.misc_flags) ? # x : ""

void k3_mhl_msc_command_done(u8 data1)
{
    u32 misc_flags = 0;
    u32 misc_flags_need = 0;

    IN_FUNCTION;

    logd("data1 = 0x%02X\n", data1);
    DECREMENT_CBUS_REFERENCE_COUNT(k3_mhl_msc_command_done);

    switch (g_mhl_tx_config.msc_last_command) {
        case MHL_READ_DEVCAP :
            k3_mhl_msc_command_read_devcap_done(data1);
            break;
        case MHL_WRITE_STAT :
            k3_mhl_msc_command_write_stat_done();
            break;
        case MHL_MSC_MSG :
            k3_mhl_msc_command_msc_msg_done();
            break;
        case MHL_WRITE_BURST :
            k3_mhl_msc_command_write_burst_done();
            break;
        case MHL_SET_INT :
            k3_mhl_msc_command_set_int_done();
            break;
        default :
            break;
    }

    misc_flags_need = FLAGS_HAVE_DEV_CATEGORY | FLAGS_HAVE_DEV_FEATURE_FLAGS
        | FLAGS_SENT_PATH_EN | FLAGS_SENT_DCAP_RDY;

    misc_flags = (FLAGS_RCP_READY & g_mhl_tx_config.misc_flags)
        | (FLAGS_HAVE_DEV_CATEGORY & g_mhl_tx_config.misc_flags)
        | (FLAGS_HAVE_DEV_FEATURE_FLAGS & g_mhl_tx_config.misc_flags)
        | (FLAGS_SENT_PATH_EN & g_mhl_tx_config.misc_flags)
        | (FLAGS_SENT_DCAP_RDY & g_mhl_tx_config.misc_flags);

    if (misc_flags == misc_flags_need
        && g_mhl_tx_config.dev_cap_cache.uc_dev_cap_cache_index >= DEVCAP_SIZE) {
        g_mhl_tx_config.misc_flags |= FLAGS_RCP_READY;

        /* Now we can entertain App commands for RCP Let app know this state */
        g_mhl_tx_config.mhl_connection_event = true;
        g_mhl_tx_config.mhl_connected = MHL_TX_EVENT_RCP_READY;

        /* send event to up-level */
        app_notify_mhl_event(g_mhl_tx_config.mhl_connected, 0);
    }

    OUT_FUNCTION;
}

/********************************************************************************
* Function:       k3_mhl_notify_connection
* Description:
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
void k3_mhl_notify_connection(bool mhl_connected)
{
    IN_FUNCTION;

    g_mhl_tx_config.mhl_connection_event = true;

    if (mhl_connected) {
        g_mhl_tx_config.rap_flags |= RAP_CONTENT_ON;
        logi("RAP CONTENT_ON\n");
        g_mhl_tx_config.mhl_connected = MHL_TX_EVENT_CONNECTION;
        g_mhl_tx_config.mhl_hpd_rsen_flags |= MHL_RSEN;
        k3_mhl_tmds_enable();
        k3_mhl_send_link_mode();
    } else {
        g_mhl_tx_config.rap_flags &= ~RAP_CONTENT_ON;
        logi("RAP CONTENT_OFF\n");
        g_mhl_tx_config.mhl_connected = MHL_TX_EVENT_DISCONNECTION;
        g_mhl_tx_config.mhl_hpd_rsen_flags &= ~MHL_RSEN;
    }

    k3_mhl_process_events();

    OUT_FUNCTION;
}

/********************************************************************************
* Function:       k3_mhl_notify_hpd_change
* Description:    Driver tells about arrival of SET_HPD or CLEAR_HPD by calling this function.
*                 Turn the content off or on based on what we got.
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
void k3_mhl_notify_hpd_change(u8 ds_hpd_status)
{
    IN_FUNCTION;

    if (0 == ds_hpd_status) {
        logi("Disable TMDS\n");
        logi("DsHPD OFF\n");
        g_mhl_tx_config.mhl_hpd_rsen_flags &= ~MHL_HPD;

        CLR_MISC_FLAG(k3_mhl_notify_hpd_change, FLAGS_EDID_READ_DONE);

        k3_mhl_tmds_control(false);

        CBus_queue.head = 0;
        CBus_queue.tail = 0;
        g_mhl_tx_config.cbus_reference_count = 0;
        g_mhl_tx_config.status_0 = 0;
        if (g_mhl_tx_config.real_disconnection) {
            g_mhl_tx_config.status_1 = 0;
        }
    } else {
        logi("Enable TMDS\n");
        logi("DsHPD ON\n");
        g_mhl_tx_config.mhl_hpd_rsen_flags |= MHL_HPD;

        g_mhl_tx_config.rap_flags |= RAP_CONTENT_ON; //Enable TMDS ouput. Supposed RAP_CONTENT_ON.
        k3_mhl_tmds_enable();
    }

    OUT_FUNCTION;
}
/********************************************************************************
* Function:       k3_mhl_notify_rgnd_mhl
* Description:
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
void k3_mhl_notify_rgnd_mhl(void)
{
    IN_FUNCTION;

    if (MHL_TX_EVENT_STATUS_PASSTHROUGH == app_notify_mhl_event(MHL_TX_EVENT_RGND_MHL, 0)) {
        // Application layer did not claim this, so send it to the driver layer
        //k3_mhl_process_rgnd();//to do //fpga cannot test
    }

    OUT_FUNCTION;
}

/********************************************************************************
* Function:       k3_mhl_got_msc_msg
* Description:    This function is called by the driver to inform of arrival of a MHL MSC_MSG  such as
                  RCP, RCPK, RCPE. To quickly return back to interrupt, this function remembers the
                  event (to be picked up by app later in task context).It is called in interrupt context
                  to meet some MHL specified timings, therefore,it should not have to call app layer
                  and do negligible processing of its own.
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:  Application shall not call this function.
*********************************************************************************/
void k3_mhl_got_msc_msg(u8 sub_command, u8 cmd_data)
{
    IN_FUNCTION;

    // Remember the event.
    g_mhl_tx_config.msc_msg_arrived = true;
    g_mhl_tx_config.msc_msg_sub_command = sub_command;
    g_mhl_tx_config.msc_msg_data = cmd_data;

    OUT_FUNCTION;
}

/********************************************************************************
* Function:      k3_mhl_got_intr
* Description:   This function is called by the driver to inform of arrival of a MHL INTERRUPT.It is
                 called in interrupt context to meet some MHL specified timings, therefore,it should
                 not have to call app layer and do negligible processing, no printfs.
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
void k3_mhl_got_intr(u8 intr_0, u8 intr_1)
{
    IN_FUNCTION;

    logd("INTERRUPT Arrived. 0x%02X, 0x%02X\n", (int)intr_0, (int)intr_1);
    // Handle DCAP_CHG INTR here
    if (MHL_INT_DCAP_CHG & intr_0) {
        if (MHL_STATUS_DCAP_RDY & g_mhl_tx_config.status_0) {
            k3_mhl_refresh_peer_devCap_entries(); //temporarily removed.
        }
    }

    if (MHL_INT_DSCR_CHG & intr_0) {
        //SiiProcessWriteBurstData(); //to do
        logd("processWriteBurstData.\n");
        // remote WRITE_BURST is complete
        CLR_MISC_FLAG(k3_mhl_got_intr, FLAGS_SCRATCHPAD_BUSY)
        app_notify_mhl_event(MHL_TX_EVENT_DSCR_CHG, 0);
    }

    if (MHL_INT_REQ_WRT & intr_0) {
        // this is a request from the sink device.
        if (FLAGS_SCRATCHPAD_BUSY & g_mhl_tx_config.misc_flags) {
            // use priority 1 to defer sending grant until local traffic is done
            k3_mhl_set_int(MHL_RCHANGE_INT, MHL_INT_GRT_WRT, 1);
        } else {
            SET_MISC_FLAG(k3_mhl_got_intr, FLAGS_SCRATCHPAD_BUSY)
            // OK to call this here, since all requests are queued
            // use priority 0 to respond immediately
            k3_mhl_set_int( MHL_RCHANGE_INT, MHL_INT_GRT_WRT, 0);
        }
    }

    if (MHL_INT_GRT_WRT & intr_0) {

    }

    if (MHL_INT_EDID_CHG & intr_1) {
        //force upstream source to read the EDID again.
        //Most likely by appropriate togggling of HDMI HPD
        //SiiDrvNotifyEdidChange();//to do
    }

    OUT_FUNCTION;
}

/********************************************************************************
* Function:      k3_mhl_got_status
* Description:   This function is called by the driver to inform of arrival of a MHL STATUS.
                 called in interrupt context to meet some MHL specified timings, therefore,it should
                 not have to call app layer and do negligible processing, no printfs.
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
void k3_mhl_got_status(u8 status_0, u8 status_1)
{
    // Handle DCAP_RDY STATUS here itself
    u8 status_change_bit_mask0, status_change_bit_mask1;

    IN_FUNCTION;

    logd("STATUS Arrived. 0x%02X, 0x%02X\n", (int)status_0, (int)status_1);
    status_change_bit_mask0 = status_0 ^ g_mhl_tx_config.status_0;
    status_change_bit_mask1 = status_1 ^ g_mhl_tx_config.status_1;

    // Remember the event.(other code checks the saved values, so save the values early, but not before the XOR operations above)
    g_mhl_tx_config.status_0 = status_0;
    g_mhl_tx_config.status_1 = status_1;

    //did DCAP_RDY change?
    if (MHL_STATUS_DCAP_RDY & status_change_bit_mask0) {
        logi("DCAP_RDY changed\n");
        if (MHL_STATUS_DCAP_RDY & status_0) {
            k3_mhl_refresh_peer_devCap_entries();
        }
    }

    // did PATH_EN change?
    if (MHL_STATUS_PATH_ENABLED & status_change_bit_mask1) {
        logi("PATH_EN changed\n");
        if (MHL_STATUS_PATH_ENABLED & status_1) {
            // OK to call this here since all requests are queued
            k3_mhl_set_path_en();
        } else {
            // OK to call this here since all requests are queued
            k3_mhl_clr_path_en();
        }
    }

    OUT_FUNCTION;
}

/********************************************************************************
* Function:     k3_mhl_rcp_send
* Description:  This function checks if the peer device supports RCP and sends rcp_key_code.
                called in interrupt context to meet some MHL specified timings, therefore,it should
                not have to call app layer and do negligible processing, no printfs.
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:       The function will return a value of true if it could successfully send the RCP subcommand
                and the key code. Otherwise false.
* Others:
*********************************************************************************/
bool k3_mhl_rcp_send(u8 rcp_key_code)
{
    bool ret_val;

    IN_FUNCTION;

    // If peer does not support, do not send RCP or RCPK/RCPE commands
    if ((0 == (MHL_FEATURE_RCP_SUPPORT & g_mhl_tx_config.msc_feature_flag))
        ||(!(FLAGS_RCP_READY & g_mhl_tx_config.misc_flags))) {
        loge("Rcp send failed\n");
        ret_val = false;
        OUT_FUNCTION;
        return ret_val;//add by zhang
    }

    ret_val = k3_mhl_tx_send_msc_msg(MHL_MSC_MSG_RCP, rcp_key_code);
    if (ret_val) {
        INCREMENT_CBUS_REFERENCE_COUNT(k3_mhl_rcp_send);
        k3_mhl_tx_drive_states();
    }

    OUT_FUNCTION;

    return ret_val;
}

/********************************************************************************
* Function:      k3_mhl_rcp_key_send
* Description:   This function sends RCPK to the peer device.
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
bool k3_mhl_rcp_key_send(u8 rcp_key_code)
{
    bool ret_val;

    IN_FUNCTION;

    ret_val = k3_mhl_tx_send_msc_msg(MHL_MSC_MSG_RCPK, rcp_key_code);
    if (ret_val) {
        k3_mhl_tx_drive_states();
    }

    OUT_FUNCTION;

    return ret_val;
}

/********************************************************************************
* Function:      k3_mhl_rapk_send
* Description:   This function sends RAPK to the peer device.
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
static bool k3_mhl_rapk_send(u8 rapk_err_code)
{
    bool ret_val;

    IN_FUNCTION;

    ret_val = k3_mhl_tx_send_msc_msg(MHL_MSC_MSG_RAPK, rapk_err_code);
    if (ret_val) {
        k3_mhl_tx_drive_states();
    }

    OUT_FUNCTION;

    return ret_val;
}

/********************************************************************************
* Function:      k3_mhl_rcp_error_send
* Description:   The function will return a value of true if it could successfully send the RCPE
                 subcommand. Otherwise false.When successful, MhlTx internally sends RCPK with
                 original (last known) keycode.
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
bool k3_mhl_rcp_error_send(u8 rcpe_error_code)
{
    bool ret_val;

    IN_FUNCTION;

    ret_val = k3_mhl_tx_send_msc_msg(MHL_MSC_MSG_RCPE, rcpe_error_code);
    if (ret_val) {
        k3_mhl_tx_drive_states();
    }

    OUT_FUNCTION;

    return ret_val;
}

/********************************************************************************
* Function:       k3_mhl_set_DCap_rdy
* Description:
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
static bool k3_mhl_set_DCap_rdy(void)
{
    bool ret;

    IN_FUNCTION;

    g_mhl_tx_config.connected_ready |= MHL_STATUS_DCAP_RDY;   // update local copy
    ret = k3_mhl_set_status(MHL_STATUS_REG_CONNECTED_RDY, g_mhl_tx_config.connected_ready);

    OUT_FUNCTION;

    return ret;
}

/********************************************************************************
* Function:      k3_mhl_set_path_en
* Description:   See 7.9.3.2 about Link Mode Status
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
bool k3_mhl_set_path_en(void)
{
    IN_FUNCTION;

    //Sometimes  PATH_EN  in status register comes later than HPD.
    //So TMDS can't be enabled in HPD processor.
    k3_mhl_tmds_enable();

    g_mhl_tx_config.link_mode |= MHL_STATUS_PATH_ENABLED;// update local copy
    k3_mhl_set_status(MHL_STATUS_REG_LINK_MODE, g_mhl_tx_config.link_mode);

    OUT_FUNCTION;

    return true;
}

/********************************************************************************
* Function:      k3_mhl_set_path_en
* Description:   See 7.9.3.2 about Link Mode Status
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
bool k3_mhl_clr_path_en(void)
{
    bool ret;

    IN_FUNCTION;

    k3_mhl_tmds_control(false);
    g_mhl_tx_config.link_mode &= ~MHL_STATUS_PATH_ENABLED;// update local copy

    ret = k3_mhl_set_status(MHL_STATUS_REG_LINK_MODE, g_mhl_tx_config.link_mode);

    OUT_FUNCTION;

    return ret;
}

/********************************************************************************
* Function:      k3_mhl_read_devcap
* Description:   This function sends a READ DEVCAP MHL command to the peer.The value of devcap
*                should be obtained by making a call to SiiGetEvents()
*                offset  Which byte in devcap register is required to be read. 0..0x0E
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:        Returns true if successful in doing so.
* Others:
*********************************************************************************/
bool k3_mhl_read_devcap(u8 offset)
{
    cbus_req_t req;
    bool ret;

    IN_FUNCTION;

    logd("devcap offset is %02x\n", offset);

    // Send MHL_READ_DEVCAP command
    req.retry_count = 2;
    req.command = MHL_READ_DEVCAP;
    req.offset_data = offset;
    req.payload_u.msg_data[0] = 0; // do this to avoid confusion

    ret = PUT_NEXT_CBUS_TRANSACTION(&req);

    OUT_FUNCTION;

    return ret;
}

/********************************************************************************
* Function:       k3_mhl_refresh_peer_devCap_entries
* Description:
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
static void k3_mhl_refresh_peer_devCap_entries(void)
{
    IN_FUNCTION;

    // only issue if no existing refresh is in progress
    if (g_mhl_tx_config.dev_cap_cache.uc_dev_cap_cache_index
        >= sizeof(g_mhl_tx_config.dev_cap_cache.auc_dev_cap_cache)) {
        g_mhl_tx_config.dev_cap_cache.uc_dev_cap_cache_index = 0;
        k3_mhl_read_devcap(g_mhl_tx_config.dev_cap_cache.uc_dev_cap_cache_index);
    }

    OUT_FUNCTION;
}

/******************************************************************************
* Function:     k3_mhl_tx_send_msc_msg
* Description:  This function sends a MSC_MSG command to the peer.
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:       Return true if successful in doing so.
* Others:
*******************************************************************************/
static bool k3_mhl_tx_send_msc_msg(u8 command, u8 cmd_data)
{
    cbus_req_t req;
    bool ret;

    IN_FUNCTION;

    memset(&req, 0, sizeof(cbus_req_t));
    // Send MSC_MSG command
    // Remember last MSC_MSG command (RCPE particularly)
    req.retry_count = 2;
    req.command = MHL_MSC_MSG;
    req.payload_u.msg_data[0] = command;
    req.payload_u.msg_data[1] = cmd_data;
    ret = PUT_NEXT_CBUS_TRANSACTION(&req);

    OUT_FUNCTION;
    return ret;
}

/******************************************************************************
* Function:     k3_mhl_read_connection_status
* Description:
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:       0: if not fully connected
                1: if fully connected
* Others:
*******************************************************************************/
u8 k3_mhl_read_connection_status(void)
{
    u8 ret;

    IN_FUNCTION;

    ret = (g_mhl_tx_config.mhl_connected >= MHL_TX_EVENT_RCP_READY) ? 1 : 0;

    OUT_FUNCTION;

    return ret;
}

/******************************************************************************
* Function:     k3_map_rcp_to_key
* Description:  This function map rcp commands to key values
* Data Accessed:
* Data Updated:
* Input:        rcp commands value
* Output:       key value
* Return:       0:success
                1:fail
* Others:
*******************************************************************************/
int k3_map_rcp_to_key(u16 rcp_val, u16 *p_key_code)
{
    IN_FUNCTION;

    if (NULL == p_key_code || rcp_val >= MHL_MAX_RCP_KEY_CODE) {
        loge("rcp value = 0x%x\n", rcp_val);
        OUT_FUNCTION;
        return -1;
    }

    *p_key_code = rcp_key_map_table[rcp_val];
    logd("key value = 0x%x\n", *p_key_code);

    OUT_FUNCTION;

    return 0;
}

/******************************************************************************
* Function:     k3_report_keycode
* Description:  This function report key values to the system
* Data Accessed:
* Data Updated:
* Input:        keycode value
* Output:
* Return:
* Others:
*******************************************************************************/
void k3_report_keycode(u16 key_code)
{
    struct input_dev *p_rcp_dev = hdmi.rcp_dev;

    IN_FUNCTION;

    logi("keycode = 0x%x\n", key_code);

    input_report_key(p_rcp_dev, key_code, 1);//1 means press down the key
    input_sync(p_rcp_dev);
    msleep(200);

    input_report_key(p_rcp_dev, key_code, 0);//0 means release the key
    input_sync(p_rcp_dev);

    OUT_FUNCTION;
}

/******************************************************************************
* Function:     app_notify_mhl_event
* Description:  This function is invoked from the MhlTx component to notify the application
                about detected events that may be of interest to it.
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:       0: if not fully connected
                1: if fully connected
* Others:       Application module must provide this function.
*******************************************************************************/
mhl_tx_notify_events_status app_notify_mhl_event(u8 event_code, u8 event_param)
{
    mhl_tx_notify_events_status retVal = MHL_TX_EVENT_STATUS_PASSTHROUGH;
    u16 keycode = KEY_RESERVED;

    IN_FUNCTION;

    switch (event_code) {
        case MHL_TX_EVENT_DISCONNECTION :
            logi("Got event = MHL_TX_EVENT_DISCONNECTION\n");
            if (g_mhl_tx_config.real_disconnection) {
                g_mhl_tx_config.disconnection_to_hdmi = true;
                g_mhl_tx_config.connection_to_hdmi = false;
                start_hdmi_process();
                hdmi.work_state = HDMI_WORK_STATE_IDLE;
            }
            break;
        case MHL_TX_EVENT_CONNECTION :
            logi("Got event = MHL_TX_EVENT_CONNECTION\n");
            //SiiSetPreferredPixelFormat(MHL_STATUS_CLK_MODE_NORMAL);//to do
            break;
        case MHL_TX_EVENT_RCP_READY :
            logi("Got event = MHL_TX_EVENT_RCP_READY\n");
            //flag_output=1;//to do -- no use
            break;
        case MHL_TX_EVENT_RCP_RECEIVED :
            k3_map_rcp_to_key(event_param, &keycode);
            k3_report_keycode(keycode);//report key to app
            // Check if we got an RCP. Application can perform the operation here
            // and send RCPK or RCPE. For now, we send the RCPK
            logi("Received an RCP key code = 0x%02X\n", (int)event_param);
            k3_mhl_rcp_key_send(event_param);
            break;
        case MHL_TX_EVENT_RCPK_RECEIVED :
            logi("Received an RCPK = 0x%02X\n", (int)event_param);
            break;
        case MHL_TX_EVENT_RCPE_RECEIVED :
            logi("Received an RCPE = 0x%02X\n", (int)event_param);
            break;
        case MHL_TX_EVENT_DCAP_CHG : {
                u8 i, myData;

                logi("MHL_TX_EVENT_DCAP_CHG:");
                for (i = 0; i < 16; ++i) {
                    if (0 == k3_mhl_get_peer_dev_cap_entry(i, &myData)) {
                        logi("0x%02x ", (int)myData);
                    } else {
                        logi("busy ");
                    }
                }
                printk("\n");
                break;
            }
        case MHL_TX_EVENT_POW_BIT_CHG :
            // handle the LEDs here
            if (event_param) {
                // POW bit 0->1 transition
                //pinSourceVbusOn = 0;
            } else {
                // POW bit 1->0 transition
                // do nothing
            }
            // Let the lower layers handle the rest.
            break;
        case MHL_TX_EVENT_RGND_MHL :
            break;
        case MHL_TX_EVENT_DSCR_CHG :
            logi("App: MHL_TX_EVENT_DSCR_CHG: \n");
            break;
        default :
            loge("Unknown event: 0x%02x\n", (int)event_code);
            break;
    }

    OUT_FUNCTION;

    return retVal;
}

/******************************************************************************
* Function:      k3_mhl_get_peer_dev_cap_entry
* Description:
* Data Accessed:
* Data Updated:
* Input:         index -- the devcap index to get
                 pdata pointer to location to write data
* Output:
* Return:        0: success
                 1: busy
* Others:
*******************************************************************************/
u8 k3_mhl_get_peer_dev_cap_entry(u8 index, u8 *pdata)
{
    IN_FUNCTION;

    if (g_mhl_tx_config.dev_cap_cache.uc_dev_cap_cache_index < DEVCAP_SIZE) {
        /* update is in progress */
        OUT_FUNCTION;
        return 1;
    }

    *pdata = g_mhl_tx_config.dev_cap_cache.auc_dev_cap_cache[index];

    OUT_FUNCTION;

    return 0;
}

#if USE_PP_MODE
#define DEVCAP_REFRESH_ONCE     (g_mhl_tx_config.uc_dev_cap_cache_index <= DEVCAP_SIZE)
#define PACKED_PIXEL_AVAILABLE  (MHL_DEV_VID_LINK_SUPP_PPIXEL & g_mhl_tx_config.dev_cap_cache.auc_dev_cap_cache[DEVCAP_OFFSET_VID_LINK_MODE])

static void examine_local_and_peer_vid_link_mode(void)
{
    IN_FUNCTION;

    /* set default values */
    g_mhl_tx_config.link_mode &= ~MHL_STATUS_CLK_MODE_MASK;
    g_mhl_tx_config.link_mode |= MHL_STATUS_CLK_MODE_NORMAL;

    /*when more modes than PPIXEL and normal are supported,
      this should become a table lookup.*/
    if (PACKED_PIXEL_AVAILABLE
        && MHL_DEV_VID_LINK_SUPP_PPIXEL & DEVCAP_VAL_VID_LINK_MODE) {
        g_mhl_tx_config.link_mode &= ~MHL_STATUS_CLK_MODE_MASK;
        g_mhl_tx_config.link_mode |= g_mhl_tx_config.preferred_clk_mode;
    }

    OUT_FUNCTION;
}

/******************************************************************************
* Function:     k3_mhl_tx_set_preferred_pixel_format
* Description:
* Data Accessed:
* Data Updated:
* Input:        clkMode - the preferred pixel format for the CLK_MODE status register
* Output:
* Return:       0: success
                -1: failure - bits were specified that are not within the mask
* Others:
*******************************************************************************/
u8 k3_mhl_tx_set_preferred_pixel_format(u8 clkMode)
{
    IN_FUNCTION;

    if (~MHL_STATUS_CLK_MODE_MASK & clkMode) {
        /*bits were specified that are not within the mask*/
        OUT_FUNCTION;
        return -1;
    }

    g_mhl_tx_config.preferred_clk_mode = clkMode;

    /*check to see if a refresh has happened since
      the last call to k3_mhl_reset_states()*/
    if (DEVCAP_REFRESH_ONCE) {
        /*check to see if DevCap cache update has
          already updated this yet*/
        if (g_mhl_tx_config.uc_dev_cap_cache_index > DEVCAP_OFFSET_VID_LINK_MODE) {
            examine_local_and_peer_vid_link_mode();
        }
    }

    OUT_FUNCTION;

    return 0;
}
bool k3_mhl_qualify_pixel_clock_for_mhl(u32 pixel_clock, u8 bits_per_pixel)
{
    u32 link_clkfreq;
    u32 max_link_clkfreq;//HZ

    IN_FUNCTION;

    link_clkfreq = (pixel_clock / 8) * (u32)bits_per_pixel;

    if (PACKED_PIXEL_AVAILABLE) {
        max_link_clkfreq = 300000000;
    } else {
        max_link_clkfreq = 225000000;
    }

    OUT_FUNCTION;

    return link_clkfreq < max_link_clkfreq;
}
void k3_mhl_tx_attempt_packed_pixel(void)
{
    u32 pixel_clock_ratio = 1;
    u32 pixel_clock_freq = 0;
    u8 packed_Pixel_needed = 0;

    IN_FUNCTION;

    k3_mhl_tx_set_preferred_pixel_format(MHL_STATUS_CLK_MODE_NORMAL);

    if ((hdmi.s3d_enabled == true)
        &&(hdmi.s3d_type == HDMI_S3D_FRAME_PACKING)) {
        //Frame Packing 3D format present--see MHL spec 5.9.2
        logd("FramePacking\n");
        pixel_clock_ratio = 2;
    } else {
        logd("NOT FramePacking\n");
    }
    k3_mhl_tx_drv_set_3D_mode(hdmi.s3d_enabled, hdmi.s3d_type);

    logi("Packed Pixel Available checking VIC:%d\n", hdmi.code);
    //check PP mode necessarity
    //pixel_clock_freq = k3_mhl_calculate_pixel_clock(hdmi.timings, hdmi.mode, hdmi.code);
    pixel_clock_freq = hdmi.timings->pixel_clock;
    // make decision about packed pixel mode here
    if (0 == pixel_clock_freq) {
        loge("We could not identify incoming timing mode,and default to non-packed-pixel.\n");
        packed_Pixel_needed=0;
    } else {
        pixel_clock_freq *= pixel_clock_ratio;
        logd("pixel clock:%lu\n", (long unsigned int)pixel_clock_freq);

        if (k3_mhl_qualify_pixel_clock_for_mhl(pixel_clock_freq, 24)) {
            logd("OK for 24 bit pixels\n");
        } else {
            if(edid_supported_yuv422(edid)) {//test by zhang :maybe use devcap link mode here
                logd("YCbCr422 support!\n");
                if (k3_mhl_qualify_pixel_clock_for_mhl(pixel_clock_freq, 16)) {
                    // enough for packed pixel
                    packed_Pixel_needed = 1;
                } else {
                    loge("We really can't do this display timing mode\n");
                }
            }
        }
    }

    k3_mhl_tx_drv_set_pixel_clk_freq(pixel_clock_freq);

    k3_mhl_tx_drv_set_packed_pixel_status(0);
    //step3: PP mode enable
    if (PACKED_PIXEL_AVAILABLE) {
        if (packed_Pixel_needed) {
            logi("Setting packed pixel mode\n");
            hdmi.tpi_v_cfg.output_color_space= TPI_COLOR_SPACE_YCBCR422;
            hdmi.avi_param.db1y_rgb_yuv422_yuv444 = INFOFRAME_AVI_DB1Y_YUV422;
            k3_mhl_tx_set_preferred_pixel_format(MHL_STATUS_CLK_MODE_PACKED_PIXEL);
            k3_mhl_tx_drv_set_packed_pixel_status(1);
        }
    }

    k3_mhl_tx_drv_apply_PPmode_settings();

    k3_mhl_send_link_mode();

    OUT_FUNCTION;
}

#endif

