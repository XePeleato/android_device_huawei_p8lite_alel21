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
 * file: k3_mhl_defs.h
 */

#ifndef __K3_MHL_DEFS_H__
#define __K3_MHL_DEFS_H__

#include <linux/types.h>

#define USE_PP_MODE 1

#define BIT0 0x01
#define BIT1 0x02
#define BIT2 0x04
#define BIT3 0x08
#define BIT4 0x10
#define BIT5 0x20
#define BIT6 0x40
#define BIT7 0x80


#define MHL_CORE_SYS HDMI_CORE_SYS

enum    // Index into pageConfig_t array (shifted left by 8)
{
    TX_PAGE_TPI = 0x0000,   // HW_TPI
    TX_PAGE_L0  = 0x0100,   // TX Legacy page 0
    TX_PAGE_L1  = 0x0200,   // TX Legacy page 1
    TX_PAGE_L2  = 0x0300,   // TX Legacy page 2
    TX_PAGE_CEC = 0x0400,   // CEC1.6
    TX_PAGE_CBUS = 0x0600,   // CBUS
};

#define PARALLEL_ADDR_TX_PAGE_TPI     MHL_CORE_SYS          // HW_TPI
#define PARALLEL_ADDR_TX_PAGE_L0      MHL_CORE_SYS          // TX Legacy page 0
#define PARALLEL_ADDR_TX_PAGE_L1      MHL_CORE_SYS + 0x400  // TX Legacy page 1
#define PARALLEL_ADDR_TX_PAGE_L2      MHL_CORE_SYS          // TX Legacy page 2
#define PARALLEL_ADDR_TX_PAGE_CEC     MHL_CORE_SYS + 0x800  // CEC1.6
#define PARALLEL_ADDR_TX_PAGE_CBUS    MHL_CORE_SYS + 0x1000 // CBUS


typedef struct
{
    u8 uc_dev_cap_cache_index;   //Buffer index of Cache
    u8 auc_dev_cap_cache[16];
}mhl_dev_cap,*p_mhl_dev_cap;

typedef enum
{
    DEVCAP_OFFSET_DEV_STATE = 0x00,
    DEVCAP_OFFSET_MHL_VERSION = 0x01,
    DEVCAP_OFFSET_DEV_CAT = 0x02,
    DEVCAP_OFFSET_ADOPTER_ID_H = 0x03,
    DEVCAP_OFFSET_ADOPTER_ID_L = 0x04,
    DEVCAP_OFFSET_VID_LINK_MODE = 0x05,
    DEVCAP_OFFSET_AUD_LINK_MODE = 0x06,
    DEVCAP_OFFSET_VIDEO_TYPE = 0x07,
    DEVCAP_OFFSET_LOG_DEV_MAP = 0x08,
    DEVCAP_OFFSET_BANDWIDTH = 0x09,
    DEVCAP_OFFSET_FEATURE_FLAG = 0x0A,
    DEVCAP_OFFSET_DEVICE_ID_H = 0x0B,
    DEVCAP_OFFSET_DEVICE_ID_L = 0x0C,
    DEVCAP_OFFSET_SCRATCHPAD_SIZE = 0x0D,
    DEVCAP_OFFSET_INT_STAT_SIZE = 0x0E,
    DEVCAP_OFFSET_RESERVED = 0x0F,
    // this one must be last
    DEVCAP_SIZE
}dev_cap_offset;

// Device Power State
#define MHL_DEV_UNPOWERED       0x00
#define MHL_DEV_INACTIVE            0x01
#define MHL_DEV_QUIET                 0x03
#define MHL_DEV_ACTIVE               0x04

// Version that this chip supports
#define MHL_VER_MAJOR       (0x02 << 4) // bits 4..7
#define MHL_VER_MINOR       0x00        // bits 0..3
#define MHL_VERSION         (MHL_VER_MAJOR | MHL_VER_MINOR)

//Device Category
#define MHL_DEV_CATEGORY_OFFSET             DEVCAP_OFFSET_DEV_CAT
#define MHL_DEV_CATEGORY_POW_BIT            BIT4

#define MHL_DEV_CAT_SINK                    0x01
#define MHL_DEV_CAT_SOURCE                  0x02
#define MHL_DEV_CAT_DONGLE                  0x03
#define MHL_DEV_CAT_SELF_POWERED_DONGLE     0x13

//Video Link Mode
#define MHL_DEV_VID_LINK_SUPPRGB444         0x01
#define MHL_DEV_VID_LINK_SUPPYCBCR444       0x02
#define MHL_DEV_VID_LINK_SUPPYCBCR422       0x04
#define MHL_DEV_VID_LINK_SUPP_PPIXEL        0x08
#define MHL_DEV_VID_LINK_SUPP_ISLANDS       0x10
#define MHL_DEV_VID_LINK_SUPP_VGA           0x20

//Audio Link Mode Support
#define MHL_DEV_AUD_LINK_2CH                0x01
#define MHL_DEV_AUD_LINK_8CH                0x02


//Feature Flag in the devcap
#define MHL_DEV_FEATURE_FLAG_OFFSET         DEVCAP_OFFSET_FEATURE_FLAG
#define MHL_FEATURE_RCP_SUPPORT             BIT0    // Dongles have freedom to not support RCP
#define MHL_FEATURE_RAP_SUPPORT             BIT1    // Dongles have freedom to not support RAP
#define MHL_FEATURE_SP_SUPPORT              BIT2    // Dongles have freedom to not support SCRATCHPAD

//Logical Dev Map
#define MHL_DEV_LD_DISPLAY                  (0x01 << 0)
#define MHL_DEV_LD_VIDEO                    (0x01 << 1)
#define MHL_DEV_LD_AUDIO                    (0x01 << 2)
#define MHL_DEV_LD_MEDIA                    (0x01 << 3)
#define MHL_DEV_LD_TUNER                    (0x01 << 4)
#define MHL_DEV_LD_RECORD                   (0x01 << 5)
#define MHL_DEV_LD_SPEAKER                  (0x01 << 6)
#define MHL_DEV_LD_GUI                      (0x01 << 7)

//Bandwidth
#define MHL_BANDWIDTH_LIMIT                 22      // 225 MHz


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

#define MHL_RCHANGE_INT                     0x20
#define MHL_DCHANGE_INT                     0x21

#define MHL_INT_DCAP_CHG                    BIT0
#define MHL_INT_DSCR_CHG                    BIT1
#define MHL_INT_REQ_WRT                     BIT2
#define MHL_INT_GRT_WRT                     BIT3
#define MHL2_INT_3D_REQ                     BIT4

// On INTR_1 the EDID_CHG is located at BIT 0
#define MHL_INT_EDID_CHG                    BIT1

#define MHL_INT_AND_STATUS_SIZE         0x33        // This contains one nibble each - max offset
#define MHL_SCRATCHPAD_SIZE             16
#define MHL_MAX_BUFFER_SIZE             MHL_SCRATCHPAD_SIZE // manually define highest number

enum
{
    MHL_MSC_MSG_RCP             = 0x10,     // RCP sub-command
    MHL_MSC_MSG_RCPK            = 0x11,     // RCP Acknowledge sub-command
    MHL_MSC_MSG_RCPE            = 0x12,     // RCP Error sub-command
    MHL_MSC_MSG_RAP             = 0x20,     // Mode Change Warning sub-command
    MHL_MSC_MSG_RAPK            = 0x21,     // MCW Acknowledge sub-command
};

#define RCPE_NO_ERROR               0x00
#define RCPE_INEEFECTIVE_KEY_CODE   0x01
#define RCPE_BUSY                   0x02

//RAPK sub commands
enum
{
    MHL_MSC_MSG_RAP_NO_ERROR                = 0x00,     // RAP No Error
    MHL_MSC_MSG_RAP_UNRECOGNIZED_ACT_CODE   = 0x01,
    MHL_MSC_MSG_RAP_UNSUPPORTED_ACT_CODE    = 0x02,
    MHL_MSC_MSG_RAP_RESPONDER_BUSY          = 0x03,
};

// MHL spec related defines
enum
{
    MHL_ACK                     = 0x33, // Command or Data byte acknowledge
    MHL_NACK                    = 0x34, // Command or Data byte not acknowledge
    MHL_ABORT                   = 0x35, // Transaction abort
    MHL_WRITE_STAT              = 0x60 | 0x80,  // 0xE0 - Write one status register strip top bit
    MHL_SET_INT                 = 0x60, // Write one interrupt register
    MHL_READ_DEVCAP             = 0x61, // Read one register
    MHL_GET_STATE               = 0x62, // Read CBUS revision level from follower
    MHL_GET_VENDOR_ID           = 0x63, // Read vendor ID value from follower.
    MHL_SET_HPD                 = 0x64, // Set Hot Plug Detect in follower
    MHL_CLR_HPD                 = 0x65, // Clear Hot Plug Detect in follower
    MHL_SET_CAP_ID              = 0x66, // Set Capture ID for downstream device.
    MHL_GET_CAP_ID              = 0x67, // Get Capture ID from downstream device.
    MHL_MSC_MSG                 = 0x68, // VS command to send RCP sub-commands
    MHL_GET_SC1_ERRORCODE       = 0x69, // Get Vendor-Specific command error code.
    MHL_GET_DDC_ERRORCODE       = 0x6A, // Get DDC channel command error code.
    MHL_GET_MSC_ERRORCODE       = 0x6B, // Get MSC command error code.
    MHL_WRITE_BURST             = 0x6C, // Write 1-16 bytes to responder's scratchpad.
    MHL_GET_SC3_ERRORCODE       = 0x6D, // Get channel 3 command error code.
};

#define MHL_RAP_CMD_POLL        0x00    // Poll
#define MHL_RAP_CONTENT_ON      0x10    // Turn content streaming ON.
#define MHL_RAP_CONTENT_OFF     0x11    // Turn content streaming OFF.

//-------------------------
#define SiI_DEVICE_ID              0x8788
#define DEVCAP_VAL_DEV_STATE       0
#define DEVCAP_VAL_MHL_VERSION     MHL_VERSION
#define DEVCAP_VAL_DEV_CAT         (MHL_DEV_CAT_SOURCE /*| MHL_DEV_CATEGORY_POW_BIT */)
#define DEVCAP_VAL_ADOPTER_ID_H    0x01
#define DEVCAP_VAL_ADOPTER_ID_L    0x42
#define DEVCAP_VAL_VID_LINK_MODE   (MHL_DEV_VID_LINK_SUPPRGB444 | MHL_DEV_VID_LINK_SUPPYCBCR444 | MHL_DEV_VID_LINK_SUPPYCBCR422 | MHL_DEV_VID_LINK_SUPP_ISLANDS | MHL_DEV_VID_LINK_SUPP_VGA| MHL_DEV_VID_LINK_SUPP_PPIXEL)
#define DEVCAP_VAL_AUD_LINK_MODE   (MHL_DEV_AUD_LINK_2CH | MHL_DEV_AUD_LINK_8CH)
#define DEVCAP_VAL_VIDEO_TYPE      0
#define DEVCAP_VAL_LOG_DEV_MAP     MHL_LOGICAL_DEVICE_MAP
#define DEVCAP_VAL_BANDWIDTH       0
#define DEVCAP_VAL_FEATURE_FLAG    (MHL_FEATURE_RCP_SUPPORT | MHL_FEATURE_RAP_SUPPORT |MHL_FEATURE_SP_SUPPORT)
#define DEVCAP_VAL_DEVICE_ID_H     0x87
#define DEVCAP_VAL_DEVICE_ID_L     0x88
#define DEVCAP_VAL_SCRATCHPAD_SIZE MHL_SCRATCHPAD_SIZE
#define DEVCAP_VAL_INT_STAT_SIZE   MHL_INT_AND_STATUS_SIZE
#define DEVCAP_VAL_RESERVED        0


#define REG_INTR_STATE (TX_PAGE_L0 | 0x0070)
#define REG_INTR1 (TX_PAGE_L0 | 0x0071)

typedef enum
{
    BIT_INTR1_RSEN_CHG  = 0x20,
    BIT_INTR1_HPD_CHG = 0x40
} intr1_bits;

#define REG_INTR1_MASK (TX_PAGE_L0 | 0x0075)

#define REG_INTR2      (TX_PAGE_L0 | 0x0072)
#define REG_INTR2_MASK (TX_PAGE_L0 | 0x0076)
#define REG_INTR3_MASK (TX_PAGE_L0 | 0x0078)
#define REG_INTR4_MASK (TX_PAGE_L0 | 0x0079)
#define REG_INTR5_MASK (TX_PAGE_L0 | 0x007A)

#define REG_TMDS_CCTRL          (TX_PAGE_L0 | 0x0080)//TMDS_CCTRL
#define TMDS_OE                 BIT4
#define REG_USB_CHARGE_PUMP_MHL (TX_PAGE_L0 | 0x00F7)
#define REG_USB_CHARGE_PUMP     (TX_PAGE_L0 | 0x00F8)

#define REG_LM_DDC      (TX_PAGE_L0 | 0x00C7)
#define DDC_GPU_REQUEST BIT0
#define DDC_TPI_SW      BIT2
#define VIDEO_MUTE_EN   BIT5
#define SW_TPI_EN       BIT7


//#if USE_PP_MODE
#define REG_MHLTX_CTR10    (TX_PAGE_L2 | 0x006D)
#define BIT_VID_MODE_m1080p_MASK    0x40
#define BIT_VID_MODE_m1080p_DISABLE 0x00
#define BIT_VID_MODE_m1080p_ENABLE  0x40


#define REG_EN_3D_CONV     (TX_PAGE_L2 | 0x00B0)  //EN_3D_CONV
#define BIT_VID_OVRRD_3DCONV_EN_MASK        0x01
#define BIT_VID_OVRRD_3DCONV_EN_NORMAL      0x00
#define BIT_VID_OVRRD_3DCONV_EN_FRAME_PACK  0x01
#define REG_CTRL_3D_CONV   (TX_PAGE_L2 | 0x00B1)  //EN_3D_CONV
//#endif


/*DDC master to read EDID*/

//--------------------------read EDID----------------------------------//

#define REG_DDC_CMD         (TX_PAGE_L0 | 0x00F3)
#define REG_DDC_STATUS      (TX_PAGE_L0 | 0x00F2)
#define DDC_FIFO_EMPTY       BIT2
#define DDC_IN_PROGRAMMING   BIT4
#define REG_DDC_ADDR        (TX_PAGE_L0 | 0x00ED)
#define REG_DDC_OFFSET      (TX_PAGE_L0 | 0x00EF)
#define REG_DDC_DIN_CNT1    (TX_PAGE_L0 | 0x00F0)
#define REG_DDC_DATA        (TX_PAGE_L0 | 0x00F4)

//-------------------------end read EDID-----------------------------//

#define REG_AUDP_TXCTRL         (TX_PAGE_L1 | 0x2F) //AUDP_TXCTRL
#define REG_AUDP_AUD_MODE       (TX_PAGE_L1 | 0x14)  //AUD_MODE
#define REG_AUDP_SPDIF_CTRL     (TX_PAGE_L1 | 0x15)  //SPDIF_CTRL
#define REG_AUDP_I2S_IN_CTRL    (TX_PAGE_L1 | 0x1D)  //I2S_IN_CTRL
#define REG_AUDP_I2S_CHST4      (TX_PAGE_L1 | 0x21)  //I2S_CHST4
#define REG_AUDP_I2S_CHST5      (TX_PAGE_L1 | 0x22)  //I2S_CHST5
#define REG_AUDP_ASRC           (TX_PAGE_L1 | 0x23)  //ASRC
#define REG_POWER_EN            (TX_PAGE_L0 | 0x0B)

#define REG_SRST        (TX_PAGE_L0 | 0x0005)       // Was 0x0005
#define REG_DISC_CTRL1  (TX_PAGE_L2 | 0x0040)       // Was 0x0090
#define REG_DISC_CTRL2  (TX_PAGE_L2 | 0x0041)       // Was 0x0091
#define REG_DISC_CTRL3  (TX_PAGE_L2 | 0x0042)       // Was 0x0092
#define REG_DISC_CTRL4  (TX_PAGE_L2 | 0x0043)       // Was 0x0093
#define REG_DISC_CTRL5  (TX_PAGE_L2 | 0x0044)       // Was 0x0094
#define REG_DISC_CTRL6  (TX_PAGE_L2 | 0x0045)       // Was 0x0095
#define REG_DISC_CTRL7  (TX_PAGE_L2 | 0x0046)       // Was 0x0096
#define REG_DISC_CTRL8  (TX_PAGE_L2 | 0x0047)       // Was 0x0097
#define REG_DISC_CTRL9  (TX_PAGE_L2 | 0x0048)
#define REG_DISC_CTRL10 (TX_PAGE_L2 | 0x0049)
#define REG_DISC_CTRL11 (TX_PAGE_L2 | 0x004A)
#define REG_DISC_STAT   (TX_PAGE_L2 | 0x004B)       // Was 0x0098
#define REG_DISC_STAT2  (TX_PAGE_L2 | 0x004C)       // Was 0x0099


#define REG_INT_CTRL (TX_PAGE_L0 | 0x007B)   //INT_CTRL
#define REG_INTRM4   (TX_PAGE_L2 | 0x0051)   // INTRM4

typedef enum
{
    BIT_INTRM4_SCDT_CHG        = BIT0,
    BIT_INTRM4_VBUS_CHG        = BIT1,
    BIT_INTRM4_MHL_EST         = BIT2,
    BIT_INTRM4_USB_EST         = BIT3,
    BIT_INTRM4_CBUS_LKOUT      = BIT4,
    BIT_INTRM4_CBUS_DISCONNECT = BIT5,
    BIT_INTRM4_RGND_DETECTION  = BIT6
} cbus_intrm4_bits;

#define REG_INTRM4_MASK       (TX_PAGE_L2 | 0x0052)   // INTRM4_MASK
#define REG_INTRM5            (TX_PAGE_L2 | 0x0053)   //INTRM5
#define REG_INTRM5_MASK       (TX_PAGE_L2 | 0x0054)   //INTRM5_MASK
#define REG_CBUS_INT_0_ENABLE (TX_PAGE_CBUS|0x0093)
#define REG_CBUS_INT_1_ENABLE (TX_PAGE_CBUS|0x0095)

#define REG_MON_USE_COMP_EN   (TX_PAGE_L2 | 0x004B)
#define REG_ZONE_CTRL_SW_RST  (TX_PAGE_L2 | 0x004C)

typedef enum
{
    BIT_ZONE_CTRL_SW_RST_SZONE_I2C_MASK = 0x30
}zone_ctrl_sw_rst_bits;

#define REG_MHLTX_CTL1 (TX_PAGE_L2 | 0x0060)       // Was 0x00A0
#define REG_MHLTX_CTL2 (TX_PAGE_L2 | 0x0061)       // Was 0x00A1
#define REG_MHLTX_CTL3 (TX_PAGE_L2 | 0x0062)       // Was 0x00A2
#define REG_MHLTX_CTL4 (TX_PAGE_L2 | 0x0063)       // Was 0x00A3
#define REG_MHLTX_CTL5 (TX_PAGE_L2 | 0x0064)       // Was 0x00A4
#define REG_MHLTX_CTL6 (TX_PAGE_L2 | 0x0065)       // Was 0x00A5
#define REG_MHLTX_CTL7 (TX_PAGE_L2 | 0x0066)       // Was 0x00A6
#define REG_MHLTX_CTL8 (TX_PAGE_L2 | 0x0067)
#define REG_MHLTX_AON_RSV0 (TX_PAGE_L2 | 0x006B)


///////////////////////////////////////////////////////////////////////////////
//
// CBUS register defintions
//
//#define BIT_DDC_ABORT  BIT2    /* Responder aborted DDC command at translation layer */
#define BIT_MSC_MSG_RCV  BIT3    /* Responder sent a VS_MSG packet (response data or command.) */
#define BIT_MSC_XFR_DONE BIT4    /* Responder sent ACK packet (not VS_MSG) */

//#define BIT_MSC_ABORT_COMMAND  BIT6    /* Command send aborted on TX side */
//#define BIT_MSC_ABOAT_RSV      BIT3    /* Responder aborted MSC command at translation layer */

#define REG_CBUS_STATUS (TX_PAGE_CBUS | 0x0091) //Reg Name:CBUS_STATUS
typedef enum
{
    BIT_CBUS_CONNECTED    = 0x0001,
    BIT_MHL_MODE          = 0x0002,
    BIT_CBUS_HPD          = 0x0004,
    BIT_MSC_HB_SUCCESS    = 0x0008,
    BIT_MHL_CABLE_PRESENT = 0x0010
} cbus_status_bits;

#define BIT_BUS_CONNECTED 0x01

#define REG_DDC_ABORT_REASON (TX_PAGE_CBUS | 0x00CB) //Reg Name:DDC_ERRORCODE

//#define REG_MSC_REQ_ABORT_REASON (TX_PAGE_CBUS | 0x009A) //Reg Name:MSC_MT_ABORT_INT

#define REG_MSC_COMMAND_START       (TX_PAGE_CBUS | 0x00B8) //MSC_COMMAND_START
#define BIT_TRANSFER_PVT_CMD        0x01
#define BIT_SEND_MSC_MSG            0x02
#define MSC_START_BIT_MSC_CMD       (0x01 << 0)
#define MSC_START_BIT_VS_CMD        (0x01 << 1)
#define MSC_START_BIT_READ_REG      (0x01 << 2)
#define MSC_START_BIT_WRITE_REG     (0x01 << 3)
#define MSC_START_BIT_WRITE_BURST   (0x01 << 4)

#define REG_MSC_CMD_OR_OFFSET    (TX_PAGE_CBUS | 0x00B9) //MSC_CMD_OR_OFFSET
#define REG_CBUS_PRI_WR_DATA_1ST (TX_PAGE_CBUS | 0x00BA) //MSC_1ST_TRANSMIT_DATA
#define REG_CBUS_PRI_WR_DATA_2ND (TX_PAGE_CBUS | 0x00BB) //MSC_2ND_TRANSMIT_DATA
#define REG_CBUS_PRI_RD_DATA_1ST (TX_PAGE_CBUS | 0x00BC) //MSC_MT_RCVD_DATA0
#define REG_CBUS_PRI_RD_DATA_2ND (TX_PAGE_CBUS | 0x00BD) //MSC_MT_RCVD_DATA1

#define REG_CBUS_PRI_VS_CMD  (TX_PAGE_CBUS | 0x00BF) //MSC_MR_MSC_MSG_RCVD_1ST_DATA
#define REG_CBUS_PRI_VS_DATA (TX_PAGE_CBUS | 0x00C0) //MSC_MR_MSC_MSG_RCVD_2ND_DATA

#define MSC_REQUESTOR_DONE_NACK (0x01 << 6)

#define REG_CBUS_MSC_RETRY_INTERVAL (TX_PAGE_CBUS | 0x00D5)     // default is 16  MSC_RETRY_INTERVAL
#define REG_CBUS_DDC_FAIL_LIMIT     (TX_PAGE_CBUS | 0x00CE)     // default is 5 CBUS_DDC_FAIL_LIMIT
#define REG_CBUS_MSC_FAIL_LIMIT     (TX_PAGE_CBUS | 0x00CF)     // default is 5 CBUS_MSC_FAIL_LIMIT

#define REG_CBUS_INT_0 (TX_PAGE_CBUS | 0x0092)  //CBUS_INT_0
typedef enum
{
    BIT_CBUS_CNX_CHG            = BIT0,
    BIT_CBUS_MSC_MT_DONE        = BIT1,
    BIT_CBUS_HPD_CHANGE         = BIT2,
    BIT_CBUS_MSC_MR_WRITE_STAT  = BIT3,/* Used */ // New :   CBUS_INT_0
    BIT_CBUS_MSC_MR_MSC_MSG     = BIT4,/* Responder sent a VS_MSG packet (response data or command.) */
    BIT_CBUS_MSC_MR_WRITE_BURST = BIT5,/* Used */ // New :   CBUS_INT_0
    BIT_CBUS_MSC_MR_SET_INT     = BIT6,
    BIT_CBUS_MSC_MT_DONE_NACK   = BIT7
} cbus_int0_bits;
#define REG_CBUS_INT_0_MASK (TX_PAGE_CBUS | 0x0093) //CBUS_INT_0_MASK

#define REG_CBUS_INT_1 (TX_PAGE_CBUS | 0x0094)  //Reg Name: CBUS_INT_1
typedef enum
{
    BIT_CBUS_CEC_ABRT            = 0x02, /*Source sent ABORT to Sink.*/ /* Responder aborted DDC command at translation layer */
    BIT_CBUS_DDC_ABRT            = 0x04, /* Responder sent a VS_MSG packet (response data or command.) */
    BIT_CBUS_MSC_ABORT           = 0x08,
    BIT_CBUS_MSC_SET_CAP_ID_RCVD = 0x10,
    BIT_CBUS_RCV_VALID           = 0x20,
    BIT_CBUS_CMD_ABORT           = 0x40, /*Source as requester, sink as responder.*/
    BIT_CBUS_MHL_CABLE_CNX_CHG   = 0x80
} cbus_int1_bits;
#define REG_CBUS_INT_1_MASK (TX_PAGE_CBUS | 0x0095)  // Reg Name:CBUS_INT_1_MASK

#define REG_CBUS_CEC_ABORT_INT (TX_PAGE_CBUS | 0x0096)
#define REG_CBUS_CEC_ABORT_INT_MASK (TX_PAGE_CBUS | 0x0097)

#define REG_CBUS_DDC_ABORT_INT   (TX_PAGE_CBUS | 0x0098)
typedef enum{
    BIT_CBUS_DDC_MAX_FAIL   = 0x01,
    BIT_CBUS_DDC_PROTO_ERR  = 0x02,
    BIT_CBUS_DDC_TIMEOUT    = 0x04,
    BIT_CBUS_DDC_PEER_ABORT = 0x80
}cbus_ddc_abort_int_bits;
#define REG_CBUS_DDC_ABORT_INT_MASK (TX_PAGE_CBUS | 0x0099)

#define REG_CBUS_MSC_MT_ABORT_INT (TX_PAGE_CBUS | 0x009A)
typedef enum{
    BIT_CBUS_MSC_MT_ABORT_INT_MAX_FAIL          = 0x01,
    BIT_CBUS_MSC_MT_ABORT_INT_PROTO_ERR         = 0x02,
    BIT_CBUS_MSC_MT_ABORT_INT_TIMEOUT           = 0x04,
    BIT_CBUS_MSC_MT_ABORT_INT_UNDEF_CMD         = 0x08,
    BIT_CBUS_MSC_MT_ABORT_INT_MSC_MT_PEER_ABORT = 0x80
}msc_mt_abort_int_bits;
#define REG_CBUS_MSC_MT_ABORT_INT_MASK (TX_PAGE_CBUS | 0x009B)

#define REG_CBUS_MSC_MR_ABORT_INT  (TX_PAGE_CBUS | 0x009C)
typedef enum{
    BIT_CBUS_MSC_MR_ABORT_INT_MAX_FAIL          = 0x01,
    BIT_CBUS_MSC_MR_ABORT_INT_PROTO_ERR         = 0x02,
    BIT_CBUS_MSC_MR_ABORT_INT_TIMEOUT           = 0x04,
    BIT_CBUS_MSC_MR_ABORT_INT_UNDEF_CMD         = 0x08,
    BIT_CBUS_MSC_MR_ABORT_INT_MSC_MR_PEER_ABORT = 0x80
}msc_mr_abort_int_bits;
#define REG_CBUS_MSC_MR_ABORT_INT_MASK (TX_PAGE_CBUS | 0x009D)

#define MSC_INT2_REQ_WRITE_MSC     (0x01 << 0) // Write REG data written.
#define MSC_INT2_HEARTBEAT_MAXFAIL (0x01 << 1) // Retry threshold exceeded for sending the Heartbeat

#define REG_MSC_WRITE_BURST_LEN (TX_PAGE_CBUS | 0x00C6)  //only for WRITE_BURST.  MSC_WRITE_BURST_DATA_LEN

#define REG_MSC_HEARTBEAT_CONTROL     (TX_PAGE_CBUS | 0x00C4)//SPeriodic heart beat. TX sends GET_REV_ID MSC command. MSC_HEARTBEAT_CONTROL
#define MSC_HEARTBEAT_PERIOD_MASK     0x0F    // bits 3..0
#define MSC_HEARTBEAT_FAIL_LIMIT_MASK 0x70    // bits 6..4
#define MSC_HEARTBEAT_ENABLE          0x80    // bit 7

#define REG_MSC_TIMEOUT_LIMIT      (TX_PAGE_CBUS | 0x00D2) //MSC_TIMEOUT
#define MSC_TIMEOUT_LIMIT_MSB_MASK 0x0F         // default is 1
#define MSC_LEGACY_BIT             (0x01 << 7)     // This should be cleared.
#define REG_MSC_COMP_CTRL          (TX_PAGE_CBUS | 0x00C7) //CBUS_MSC_COMPATIBILITY_CONTROL

#define REG_CBUS_CTRL1 (TX_PAGE_CBUS | 0x00A0) // CBUS_LINK_CONTROL_1
#define REG_CBUS_CTRL2 (TX_PAGE_CBUS | 0x00A1) // CBUS_LINK_CONTROL_2
#define REG_CBUS_CTRL3 (TX_PAGE_CBUS | 0x00A2) // CBUS_LINK_CONTROL_3
#define REG_CBUS_CTRL4 (TX_PAGE_CBUS | 0x00A3) // CBUS_LINK_CONTROL_4
#define REG_CBUS_CTRL5 (TX_PAGE_CBUS | 0x00A4) // CBUS_LINK_CONTROL_5
#define REG_CBUS_CTRL6 (TX_PAGE_CBUS | 0x00A5) // CBUS_LINK_CONTROL_6
#define REG_CBUS_CTRL7 (TX_PAGE_CBUS | 0x00A6) // CBUS_LINK_CONTROL_7

#define REG_CBUS_STAT1 (TX_PAGE_CBUS | 0x00AD) // CBUS_LINK_STATUS_0
#define REG_CBUS_STAT2 (TX_PAGE_CBUS | 0x00AE) // CBUS_XMIT_ERR_CNT
#define REG_CBUS_CTRL8 (TX_PAGE_CBUS | 0x00A7) // CBUS_LINK_CONTROL_8
#define REG_CBUS_CTRL9 (TX_PAGE_CBUS | 0x00A8) // CBUS_LINK_CONTROL_9
#define REG_CBUS_CTRL10 (TX_PAGE_CBUS | 0x00A9)// CBUS_LINK_CONTROL_10
#define REG_CBUS_CTRL11 (TX_PAGE_CBUS | 0x00AA)// CBUS_LINK_CONTROL_11

#define REG_CBUS_ACK_CONTROL (TX_PAGE_CBUS | 0x00AB) // CBUS_ACK_CONTROL
#define REG_CBUS_CAL_CONTROL (TX_PAGE_CBUS | 0x00AC) // CBUS_CAL_CONTROL


#define REG_CBUS_DEVICE_CAP_0 (TX_PAGE_CBUS | 0x0000) //MHL_DEVCAP_0
#define REG_CBUS_DEVICE_CAP_1 (TX_PAGE_CBUS | 0x0001) //MHL_DEVCAP_1
#define REG_CBUS_DEVICE_CAP_2 (TX_PAGE_CBUS | 0x0002) //MHL_DEVCAP_2
#define REG_CBUS_DEVICE_CAP_3 (TX_PAGE_CBUS | 0x0003) //MHL_DEVCAP_3
#define REG_CBUS_DEVICE_CAP_4 (TX_PAGE_CBUS | 0x0004) //MHL_DEVCAP_4
#define REG_CBUS_DEVICE_CAP_5 (TX_PAGE_CBUS | 0x0005) //MHL_DEVCAP_5
#define REG_CBUS_DEVICE_CAP_6 (TX_PAGE_CBUS | 0x0006) //MHL_DEVCAP_6
#define REG_CBUS_DEVICE_CAP_7 (TX_PAGE_CBUS | 0x0007) //MHL_DEVCAP_7
#define REG_CBUS_DEVICE_CAP_8 (TX_PAGE_CBUS | 0x0008) //MHL_DEVCAP_8
#define REG_CBUS_DEVICE_CAP_9 (TX_PAGE_CBUS | 0x0009) //MHL_DEVCAP_9
#define REG_CBUS_DEVICE_CAP_A (TX_PAGE_CBUS | 0x000A) //MHL_DEVCAP_A
#define REG_CBUS_DEVICE_CAP_B (TX_PAGE_CBUS | 0x000B) //MHL_DEVCAP_B
#define REG_CBUS_DEVICE_CAP_C (TX_PAGE_CBUS | 0x000C) //MHL_DEVCAP_C
#define REG_CBUS_DEVICE_CAP_D (TX_PAGE_CBUS | 0x000D) //MHL_DEVCAP_D
#define REG_CBUS_DEVICE_CAP_E (TX_PAGE_CBUS | 0x000E) //MHL_DEVCAP_E
#define REG_CBUS_DEVICE_CAP_F (TX_PAGE_CBUS | 0x000F) //MHL_DEVCAP_F

#define REG_CBUS_SET_INT_0 (TX_PAGE_CBUS | 0x0020) //MHL_INT_0
#define REG_CBUS_SET_INT_1 (TX_PAGE_CBUS | 0x0021) //MHL_INT_1

#define REG_CBUS_WRITE_STAT_0 (TX_PAGE_CBUS | 0x0030) //MHL_STAT_0
#define REG_CBUS_WRITE_STAT_1 (TX_PAGE_CBUS | 0x0031) //MHL_STAT_1
#define REG_CBUS_WRITE_STAT_2 (TX_PAGE_CBUS | 0x0032) //MHL_STAT_2
#define REG_CBUS_WRITE_STAT_3 (TX_PAGE_CBUS | 0x0033) //MHL_STAT_3
#define REG_CBUS_SCRATCHPAD_0 (TX_PAGE_CBUS | 0x0040) //MHL_SCRPAD_0

#define REG_CBUS_SET_INT_0_MASK (TX_PAGE_CBUS | 0x0080) // MHL_INT_*_MASK
#define REG_CBUS_SET_INT_1_MASK (TX_PAGE_CBUS | 0x0081)

#endif  // __K3_DRV_MHL_TX_H__

