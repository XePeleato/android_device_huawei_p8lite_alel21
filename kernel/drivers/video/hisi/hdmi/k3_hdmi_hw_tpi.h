/***********************************************************************************/
/*  Copyright (c) 2002-2009, Silicon Image, Inc.  All rights reserved.             */
/*  No part of this work may be reproduced, modified, distributed, transmitted,    */
/*  transcribed, or translated into any language or computer format, in any form   */
/*  or by any means without written permission of: Silicon Image, Inc.,            */
/*  1060 East Arques Avenue, Sunnyvale, California 94085                           */
/***********************************************************************************/



// TPI Video Mode Data
//====================
#ifndef _K3_HDMI_HW_TPI_H_
#define _K3_HDMI_HW_TPI_H_
#include "k3_hdmi_hw.h"
#include "k3_mhl_tx.h"
#include "k3_drv_mhl_tx.h"

#define DTS_COMP_HDMI_NAME "hisilicon,k3hdmi"

#define FPGA_VIDEO_TEST
#ifdef FPGA_VIDEO_TEST
typedef enum
{
    LOGIC_27M = 0,
    LOGIC_74M,
    LOGIC_148M,
    LOGIC_54M,
} VIDEO_FPGA_TPYE;

#define FPGA_VIDEO_TEST_TYPE LOGIC_27M;
//#define FPGA_VIDEO_TEST_TYPE LOGIC_74M;
  #endif

//#define FPGA_AUDIO_TEST
#ifdef FPGA_AUDIO_TEST
#include "hdmitest.h"
/*
48k audio ---use 27m logic!!
96k audio ---use 74m logic!!
*/
#define FPGA_AUDIO_TEST_TYPE TX3_48K_I2S
//#define FPGA_AUDIO_TEST_TYPE TX3_48K_SPDIF
//#define FPGA_AUDIO_TEST_TYPE TX3_96K_I2S
//#define FPGA_AUDIO_TEST_TYPE TX3_96K_SPDIF
//#define FPGA_AUDIO_TEST_TYPE TX3_48K_DIRECT_SPDIF
#endif

#define HDCP_WRITE_TEST 0



// COMMON
//========================
#define MSBIT                   0x80
#define LSBIT                   0x01

#define BIT_0                   0x01
#define BIT_1                   0x02
#define BIT_2                   0x04
#define BIT_3                   0x08
#define BIT_4                   0x10
#define BIT_5                   0x20
#define BIT_6                   0x40
#define BIT_7                   0x80

#define TWO_LSBITS               0x03
#define THREE_LSBITS            0x07
#define FOUR_LSBITS              0x0F
#define FIVE_LSBITS               0x1F
#define SEVEN_LSBITS            0x7F
#define TWO_MSBITS              0xC0
#define EIGHT_BITS                0xFF
#define BYTE_SIZE                  0x08
#define BITS_1_0                    0x03
#define BITS_2_1                    0x06
#define BITS_2_1_0                0x07
#define BITS_3_2                    0x0C
#define BITS_4_3_2                0x1C
#define BITS_5_4                    0x30
#define BITS_5_4_3                0x38
#define BITS_6_5                   0x60
#define BITS_6_5_4               0x70
#define BITS_7_6                   0xC0
#define LOW_BYTE                0x00FF

#define T_RES_CHANGE_DELAY      128         // delay between turning TMDS bus off and changing output resolution

// Interrupt Masks
//================
#define HOT_PLUG_EVENT          0x01
#define RX_SENSE_EVENT           0x02
#define HOT_PLUG_STATE          0x04
#define RX_SENSE_STATE           0x08

#define AUDIO_ERROR_EVENT        0x10
#define SECURITY_CHANGE_EVENT 0x20
#define V_READY_EVENT                0x40
#define HDCP_CHANGE_EVENT       0x80

#define NON_MASKABLE_INT         0xFF

// TPI Control Masks
// =================

#define CS_HDMI_RGB             0x00
#define CS_DVI_RGB                0x03
#define OUTPT_MODE_DVI       0x00
#define OUTPT_MODE_HDMI    0x01

// Pixel Repetition Masks
//=======================
#define BIT_BUS_24                  0x20
#define BIT_EDGE_RISE            0x10
#define BIT_FULL_PIXEL_WIDE 0x20

//Audio Maps
//==========
#define BIT_AUDIO_MUTE         0x10

// Input/Output Format Masks
//==========================
#define BITS_IN_RGB                 0x00
#define BITS_IN_YCBCR444        0x01
#define BITS_IN_YCBCR422        0x02

#define BITS_IN_AUTO_RANGE     0x00
#define BITS_IN_FULL_RANGE      0x04
#define BITS_IN_LTD_RANGE       0x08

#define BIT_EN_DITHER_10_8      0x40
#define BIT_EXTENDED_MODE      0x80

#define BITS_OUT_RGB                0x00
#define BITS_OUT_YCBCR444       0x01
#define BITS_OUT_YCBCR422       0x02

#define BITS_OUT_AUTO_RANGE    0x00
#define BITS_OUT_FULL_RANGE     0x04
#define BITS_OUT_LTD_RANGE      0x08

#define BIT_BT_709              0x10


// DE Generator Masks
//===================
#define BIT_EN_DE_GEN           0x40
#define DE                                0x00
#define DeDataNumBytes          12

// Embedded Sync Masks
//====================
#define BIT_EN_SYNC_EXTRACT    0x40
#define EMB                                  0x80
#define EmbDataNumBytes             8


// Audio Modes
//============
#define AUD_PASS_BASIC           0x00
#define AUD_PASS_ALL               0x01
#define AUD_DOWN_SAMPLE       0x02
#define AUD_DO_NOT_CHECK      0x03

#define REFER_TO_STREAM_HDR   0x00
//#define TWO_CHANNELS            0x00
#define TWO_CHANNELS             0x01  //CEA-861-E page 49  - Fix for bug #19948
//#define EIGHT_CHANNELS          0x01
#define EIGHT_CHANNELS           0x07  //CEA-861-E page 49  - Fix for bug #19948
#define AUD_IF_SPDIF                0x40
#define AUD_IF_I2S                    0x80
#define AUD_IF_DSD                   0xC0
#define AUD_IF_HBR                   0x04

#define TWO_CHANNEL_LAYOUT      0x00
#define EIGHT_CHANNEL_LAYOUT    0x20


// Configuration File Constants
//=============================
#define CONFIG_DATA_LEN         0x10
#define IDX_VIDEO_MODE          0x00

// I2C Slave Addresses
//====================
// see include/i2c_slave_addrs.h

// DDC Bus Addresses
//==================
#define DDC_BSTATUS_ADDR_L      (0x41*4)
#define DDC_BSTATUS_ADDR_H      (0x42*4)
#define DDC_KSV_FIFO_ADDR         (0x43*4)
#define KSV_ARRAY_SIZE               128

// DDC Bus Bit Masks
//==================
#define BIT_DDC_HDMI                0x80
#define BIT_DDC_REPEATER        0x40
#define BIT_DDC_FIFO_RDY         0x20
#define DEVICE_COUNT_MASK      0x7F

// DDC Operation
#define TPI_HW_DBG7                (0x7F*4)
#define REG_HW_DDC_MASTER_MASK  0x80
#define TPI_HW_OPT3                (0xBB*4)
#define DDC_DELAY_CNT            (0xF6*4)

//CBUS Control
#define CBUS_DISC_CTRL1         (0x40*4)
#define CBUS_DISC_CTRL2         (0x41*4)
#define CBUS_DISC_CTRL3         (0x42*4)
#define CBUS_DISC_CTRL4         (0x43*4)

// KSV Buffer Size
//================
#define DEVICE_COUNT            128    // May be tweaked as needed

// InfoFrames
//===========
#define BITS_OUT_FORMAT         0x60   // Y1Y0 field - in InfoFrame DByte 1

#define _4_To_3                  0x10   // Aspect ratio - 4:3  in InfoFrame DByte 2
#define _16_To_9                0x20   // Aspect ratio - 16:9 in InfoFrame DByte 2
#define SAME_AS_AR           0x08   // R3R2R1R0 - in AVI InfoFrame DByte 2
#define ACT_FORMAT_INFO_VALID   0x10   // A0 - in AVI Infoframe DByte 1

#define BT_601                  0x40
#define BT_709                  0x80

#define TYPE_GBD_INFOFRAME      0x0A

#define ENABLE_AND_REPEAT        0xC0

#define EN_AND_RPT_MPEG           0xC3
#define DISABLE_MPEG             0x03

#define EN_AND_RPT_HDMI_VSIF    0xC6
#define DISABLE_HDMI_VSIF           0x06

#define EN_AND_RPT_AUDIO        0xC2
#define DISABLE_AUDIO               0x02

#define EN_AND_RPT_AVI          0xC0   // Not normally used.  Write to TPI 0x19 instead
#define DISABLE_AVI                 0x00   // But this is used to Disable

#define NEXT_FIELD                0x80
#define GBD_PROFILE             0x00
#define AFFECTED_GAMUT_SEQ_NUM  0x01

#define ONLY_PACKET             0x30
#define CURRENT_GAMUT_SEQ_NUM   0x01

// FPLL Multipliers:
//==================

#define X1                      0x01

// 3D Constants
//=============

#define VSI_3D_STRUC_PRESENT    0x40

// 3D_Stucture Constants
//=======================
#define FRAME_PACKING           0x00
#define FIELD_ALTERNATIVE     0x01
#define LINE_ALTERNATIVE       0x02
#define SIDE_BY_SIDE_FULL      0x03
#define L_PLUS_DEPTH              0x04
#define L_PLUS_DEPTH_PLUS_GRAPHICS   0x05
#define SIDE_BY_SIDE_HALF       0x08

// 3D_Ext_Data Constants
//======================
#define NO_3D_SUPPORT     0x0F
// These values are from HDMI Spec 1.4 Table H-2
#define VMD_3D_FRAMEPACKING_MAP    0x0F
#define VMD_3D_EXT_DATA_MAP            0x70
#define VMD_3D_FRAMEPACKING            0x00
#define VMD_3D_FIELDALTERNATIVE      0x01
#define VMD_3D_LINEALTERNATIVE        0x02
#define VMD_3D_SIDEBYSIDEFULL          0x03
#define VMD_3D_LDEPTH                        0x04
#define VMD_3D_LDEPTHGRAPHICS         0x05
#define VMD_3D_SIDEBYSIDEHALF           0x08

#define VMD_HORIZ_ODD_LEFT_ODD_RIGHT      0x00
#define VMD_HORIZ_ODD_LEFT_EVEN_RIGHT     0x01
#define VMD_HORIZ_EVEN_LEFT_ODD_RIGHT     0x02
#define VMD_HORIZ_EVEN_LEFT_EVEN_RIGHT    0x03
#define VMD_QUINCUNX_ODD_LEFT_EVEN_RIGHT  0x04
#define VMD_QUINCUNX_ODD_LEFT_ODD_RIGHT   0x05
#define VMD_QUINCUNX_EVEN_LEFT_ODD_RIGHT  0x06
#define VMD_QUINCUNX_EVEN_LEFT_EVEN_RIGHT 0x07


//===================
// InfoFrames
// InfoFrames types according to PR
#define AVI       0x00
#define SPD      0x01
#define AUDIO  0x02
#define MPEG    0x03
#define GEN_1  0x04
#define GEN_2  0x05
#define HDMI_VSIF  0x06
#define GBD             0x07

// InfoFrames types according to CEA-861-E spec
#define TYPE_HDMI_VSIF_INFOFRAMES    0x81
#define TYPE_AVI_INFOFRAMES               0x82
#define TYPE_SPD_INFOFRAMES              0x83
#define TYPE_AUDIO_INFOFRAMES          0x84
#define TYPE_MPEG_INFOFRAMES            0x85
#define TYPE_GBD_INFOFRAMES            0x0A
#define TYPE_ISRC1_INFOFRAMES           0x05
#define TYPE_ISRC2_INFOFRAMES           0x06

#define INFOFRAMES_BUFF_LENGTH        0x1F
#define AUDIO_INFOFRAMES_LENGTH     0x0A
#define SPD_INFOFRAMES_LENGTH         0x19
#define MPEG_INFOFRAME_LENGTH        0x0A // 10 bytes
#define MISC_INFOFRAMES_LENGTH       0x1B
#define PACKET_INFOFRAMES_LENGTH   0x1C

#define AVI_INFOFRAMES_VERSION       0x02
#define MISC_INFOFRAMES_VERSION     0x01

// Size of InfoFrame Data types
#define MAX_SIZE_INFOFRAME_DATA      0x22
#define SIZE_AVI_INFOFRAME                 0x0E // 14 bytes
#define SIZE_SPD_INFOFRAME                0x19 // 25 bytes
#define SIZE_AUDIO_INFOFRAME            0x0E // Audio (n = 14 bytes); according to PR.
#define SIZE_MPRG_HDMI_INFOFRAME   0x1B // 27 bytes
#define SIZE_MPEG_INFOFRAME             0x0A // 10 bytes
#define SIZE_PACKET_INFOFRAME          0x1C      // 28 bytes

#define AVI_INFOFRM_OFFSET                (0x0C*4)
#define PACKET_INFOFRM_OFFSET         (0xC3*4)
#define OTHER_INFOFRM_OFFSET          (0xC4*4)
#define TPI_INFOFRAME_ACCESS_REG   (0xBF*4)

// Serial Communication Buffer constants
#define MAX_COMMAND_ARGUMENTS       50
#define GLOBAL_BYTE_BUF_BLOCK_SIZE  131


//------------------------------------------------------------------------------
// Video Mode Constants
//------------------------------------------------------------------------------
#define VMD_CS_COMPRESSION_STANDARD          0x00
#define VMD_CS_COMPRESSION_OPTIONAL             0x01

#define VMD_ASPECT_RATIO_4x3        0x01
#define VMD_ASPECT_RATIO_16x9      0x02

#define VMD_PIXEL_REP_MAP               0x0F

#define VMD_COLOR_SPACE_MAP       0x03
//#define VMD_COLOR_SPACE_RGB   0x00
////#define VMD_COLOR_SPACE_YCBCR422       0x02
//#define VMD_COLOR_SPACE_YCBCR444            0x01
#define VMD_COLOR_SPACE_BLACK_MODE       0x03


#define VMD_COLOR_DEPTH_MAP     0xC0
#define VMD_COLOR_DEPTH_8BIT     0x00
//#define VMD_COLOR_DEPTH_10BIT     0x02
//#define VMD_COLOR_DEPTH_12BIT     0x03
//#define VMD_COLOR_DEPTH_16BIT     0x01

#define VMD_VIDEO_RANGE_MAP         0x0C
#define VMD_VIDEO_RANGE_AUTO       0x00
#define VMD_VIDEO_RANGE_FULL        0x04
#define VMD_VIDEO_RANGE_LIMITED   0x80

#define VMD_HDCP_NOT_AUTHENTICATED      0x00
#define VMD_HDCP_AUTHENTICATED              0x01

#define VMD_HDMIFORMAT_CEA_VIC      0x00
#define VMD_HDMIFORMAT_HDMI_VIC    0x01
#define VMD_HDMIFORMAT_3D               0x02
#define VMD_HDMIFORMAT_PC               0x03



//------------------------------------------------------------------------------
// Input Color Space Constants
//------------------------------------------------------------------------------
//YCbCr444/422 value have to follow plum register(TPI_09/0A) definitions.


//------------------------------------------------------------------------------
// Input Range Expansion Constants
//------------------------------------------------------------------------------
#define IVRX_AUTO                         0x00
#define IVRX_ON                             0x04
#define IVRX_OFF                            0x80

//------------------------------------------------------------------------------
// Output Color Space Constants
//------------------------------------------------------------------------------

#define OCS_YC422_8BIT                      0xC0    // YCbCr 4:2:2 8  bit
#define OCS_YC422_10BIT                     0xC8    // YCbCr 4:2:2 10 bit
#define OCS_YC422_MUX_8BIT               0xE0    // YCbCr 4:2:2 8  bit multiplexed
#define OCS_YC422_MUX_10BIT             0xE8    // YCbCr 4:2:2 10 bit multiplexed
#define OCS_YC422_MUX_8BIT_EMBED_SYNC       0xF0    // YCbCr 4:2:2 8  bit multiplexed with embedded sync
#define OCS_YC422_MUX_10BIT_EMBEDD_SYNC   0xF8    // YCbCr 4:2:2 10 bit multiplexed with embedded sync

//CTRL
//=========================
#define TPI_SW_RST_ADDR (0x40*4)   /* TPI Soft Reset Register */
#define TPI_PDO_MASK         0x80
#define TPI_P_STABLE_MASK    0x40
#define TPI_REG_DC_PKT_EN_MASK    0x04
#define TPI_REG_FIFO_RST_MASK       0x02
#define TPI_REG_SW_RST_MASK         0x01
#define TPI_SW_RST_REQUEST   0x01
#define TPI_SW_RST_RELEASE    0x00


// EDID
//=========================
#define EDID_BLOCK_0_OFFSET 0x00
#define EDID_BLOCK_1_OFFSET 0x80

#define EDID_BLOCK_SIZE 128
#define EDID_HDR_NO_OF_FF 0x06
#define NUM_OF_EXTEN_ADDR 0x7E

// Pixel Repetition Data
//======================

#define TPI_VIDEO_MODE_DATA_REG       (0x00*4)
#define TPI_VIDEO_TCLK_SEL_MASK         (BIT_7|BIT_6)

#define TPI_VERT_FREQ_LSB                    (0x02*4)
#define TPI_VERT_FREQ_MSB                   (0x03*4)

#define TPI_TOTAL_PIX_LSB                     (0x04*4)
#define TPI_TOTAL_PIX_MSB                    (0x05*4)

#define TPI_TOTAL_LINES_LSB                 (0x06*4)
#define TPI_TOTAL_LINES_MSB                (0x07*4)

/*
Pixel Repetition Factor2
0000 Pixel data is not replicated
0001 Pixels are sent two times each
0011 Pixels are sent four times each
All others RSVD
*/
#define TPI_PIX_REPETITION                   (0x08*4)

// TPI VIDEO
#define TPI_VID_BLANK1_ADDR (0x4B*4)       /* Video Blank #1 Register */
#define TPI_VID_BLANK2_ADDR (0x4C*4)       /* Video Blank #2 Register */
#define TPI_VID_BLANK3_ADDR (0x4D*4)       /* Video Blank #3 Register */

#define TPI_INDEXED_PAGE_REG      (0xBC*4)
#define TPI_INDEXED_OFFSET_REG  (0xBD*4)
#define TPI_INDEXED_VALUE_REG   (0xBE*4)

#define TPI_INDEXED_PAGE_0 0x01
#define TPI_INDEXED_PAGE_1 0x02
#define TPI_INDEXED_PAGE_2 0x03

#define TPI_INDEXED_PAGE_3 0x04//need to change

// TPI AVI Input and Output Format Data
//=====================================

/// AVI Input Format Data ================================================= ///

#define TPI_INPUT_FORMAT_REG            (0x09*4)

//Finish this...

#define INPUT_COLOR_SPACE_MASK      (BIT_1 | BIT_0)
#define INPUT_COLOR_SPACE_RGB         (0x00)
#define INPUT_COLOR_SPACE_YCBCR444    (0x01)
#define INPUT_COLOR_SPACE_YCBCR422    (0x02)
#define INPUT_COLOR_SPACE_BLACK_MODE    (0x03)

/// AVI Output Format Data ================================================ ///

#define TPI_OUTPUT_FORMAT_REG      (0x0A*4)
#define TPI_DITHER_MODE_MASK 0xC0
#define TPI_ENDITHER_MASK    0x20
#define TPI_CSCMODE709_MASK  0x10
#define TPI_OUTPUT_QUAN_RANGE_MASK   0x0C
#define TPI_OUTPUT_FORMAT_MASK   0x03

#define TPI_YC_Input_Mode      (0x0B*4)


// TPI InfoFrame related constants
#define TPI_AVI_INFO_REG_ADDR       (0x0C*4) // AVI InfoFrame Checksum
#define TPI_INFO_FRAME_REG_OFFSET    (0xC4*4)

// TPI AVI InfoFrame Data
//=======================

#define TPI_AVI_BYTE_0          (0x0C*4)
#define TPI_AVI_BYTE_1          (0x0D*4)
#define TPI_AVI_BYTE_2          (0x0E*4)
#define TPI_AVI_BYTE_3          (0x0F*4)
#define TPI_AVI_BYTE_4          (0x10*4)
#define TPI_AVI_BYTE_5          (0x11*4)

//#define TPI_AUDIO_BYTE_0   (0xBF)

#define TPI_INFO_FRM_DBYTE5     (0xC8*4)
#define TPI_INFO_FRM_DBYTE6     (0xC9*4)

#define TPI_END_TOP_BAR_LSB    (0x12*4)
#define TPI_END_TOP_BAR_MSB   (0x13*4)

#define TPI_START_BTM_BAR_LSB   (0x14*4)
#define TPI_START_BTM_BAR_MSB   (0x15*4)

#define TPI_END_LEFT_BAR_LSB    (0x16*4)
#define TPI_END_LEFT_BAR_MSB    (0x17*4)

#define TPI_END_RIGHT_BAR_LSB   (0x18*4)
#define TPI_END_RIGHT_BAR_MSB   (0x19*4)

// Colorimetry
//============
#define SET_EX_COLORIMETRY      (0x0C*4)  // Set TPI_AVI_BYTE_2 to extended colorimetry and use
//TPI_AVI_BYTE_3

// ===================================================== //

#define TPI_SYSTEM_CONTROL_DATA_REG      (0x1A*4)

#define LINK_INTEGRITY_MODE_MASK                  (BIT_6)
#define LINK_INTEGRITY_STATIC      (0x00)
#define LINK_INTEGRITY_DYNAMIC  (0x40)

#define TMDS_OUTPUT_CONTROL_MASK         (BIT_4)
#define TMDS_OUTPUT_CONTROL_ACTIVE      (0x00)
#define TMDS_OUTPUT_CONTROL_POWER_DOWN    (0x10)

#define AV_MUTE_MASK        (BIT_3)
#define AV_MUTE_NORMAL   (0x00)
#define AV_MUTE_MUTED      (0x08)

#define DDC_BUS_REQUEST_MASK    (BIT_2)
#define DDC_BUS_REQUEST_NOT_USING      (0x00)
#define DDC_BUS_REQUEST_REQUESTED    (0x04)

#define DDC_BUS_GRANT_MASK       (BIT_1)
#define DDC_BUS_GRANT_NOT_AVAILABLE       (0x00)
#define DDC_BUS_GRANT_GRANTED     (0x02)

#define OUTPUT_MODE_MASK     (BIT_0)
#define OUTPUT_MODE_DVI          (0x00)
#define OUTPUT_MODE_HDMI       (0x01)

#define HDMI_CONNECT            0x01
#define HDMI_DISCONNECT         0x02
#define HDMI_FIRST_HPD          0x08
#define HDMI_HDCP_AUTH_STATUS_CHANGE    0x80
#define HDMI_HDCP_SECURITY_CHANGE       0x20

#define TPI_REG_BANK0    (0xFF*4)
// TPI Identification Registers
//=============================

#define TPI_DEVICE_ID     (0x1B*4)
#define TPI_DEVICE_REV_ID       (0x1C*4)

#define TPI_RESERVED2       (0x1D*4)

// ===================================================== //

#define TPI_DEVICE_POWER_STATE_CTRL_REG   (0x1E*4)

#define CTRL_PIN_CONTROL_MASK                           (BIT_4)
#define CTRL_PIN_TRISTATE                                         (0x00)
#define CTRL_PIN_DRIVEN_TX_BRIDGE                    (0x10)

#define TX_POWER_STATE_MASK         (BIT_1 | BIT_0)
#define TX_POWER_STATE_D0               (0x00)
#define TX_POWER_STATE_D1                (0x01)
#define TX_POWER_STATE_D2                (0x02)
#define TX_POWER_STATE_D3                (0x03)

// Configuration of I2S Interface
//===============================

#define TPI_I2S_EN          (0x1F*4)

#define TPI_I2S_IN_CFG       (0x20*4)
#define TPI_MCLK_IN_MODE_MASK      (BIT_4|BIT_5|BIT_6)
#define TPI_SCK_EDGE_MASK                (BIT_7)

#define TPI_AUDIO_SD_ENABLE_MASK        (BIT_7)
#define TPI_AUDIO_SD_ENABLE                 (BIT_7)
#define TPI_AUDIO_SD_DISABLE                (0x00)

#define TPI_AUDIO_SD_MASK               (BIT_4|BIT_5)
#define TPI_AUDIO_SD_SD0                (0x00)
#define TPI_AUDIO_SD_SD1                (BIT_4)
#define TPI_AUDIO_SD_SD2                (BIT_5)
#define TPI_AUDIO_SD_SD3                (BIT_4|BIT_5)

#define TPI_I2S_AUDIO_DOWN_SAMPLE_MASK    (BIT3)
#define TPI_I2S_AUDIO_DOWN_SAMPLE_ENABLE    (BIT3)
#define TPI_I2S_AUDIO_DOWN_SAMPLE_DISABLE    (0x00)

#define TPI_AUDIO_SD_FIFO_MASK            (BIT_0|BIT_1)
#define TPI_AUDIO_SD_FIFO0                (0x00)
#define TPI_AUDIO_SD_FIFO1                (BIT_0)
#define TPI_AUDIO_SD_FIFO2                (BIT_1)
#define TPI_AUDIO_SD_FIFO3                (BIT_0|BIT_1)

#define TPI_AUDIO_FS_REFER                (0x00)
#define TPI_AUDIO_FS_32000                (BIT_3)
#define TPI_AUDIO_FS_44100                (BIT_4)
#define TPI_AUDIO_FS_48000                (BIT_3|BIT_4)
#define TPI_AUDIO_FS_88200                (BIT_5)
#define TPI_AUDIO_FS_96000                (BIT_5|BIT_3)
#define TPI_AUDIO_FS_176400               (BIT_5|BIT_4)
#define TPI_AUDIO_FS_192000               (BIT_5|BIT_4|BIT_3)

#define TPI_AUDIO_I2S_SIZE_16                (BIT_1)
#define TPI_AUDIO_I2S_SIZE_20                (BIT_1|BIT_0)
#define TPI_AUDIO_I2S_SIZE_24                (BIT_3|BIT_1|BIT_0)

#define TPI_AUDIO_CH_REFER             (0x00)
#define TPI_AUDIO_CH_2                    (BIT_0)
#define TPI_AUDIO_CH_3                    (BIT_1)
#define TPI_AUDIO_CH_4                    (BIT_0|BIT_1)
#define TPI_AUDIO_CH_5                    (BIT_2)
#define TPI_AUDIO_CH_6                    (BIT_2|BIT_0)
#define TPI_AUDIO_CH_7                    (BIT_2|BIT_1)
#define TPI_AUDIO_CH_8                    (BIT_2|BIT_1|BIT_0)

#define TPI_AUDIO_I2S_FS_32000                (BIT_0|BIT_1)
#define TPI_AUDIO_I2S_FS_44100                (0x00)
#define TPI_AUDIO_I2S_FS_48000                (BIT_1)
#define TPI_AUDIO_I2S_FS_88200                (BIT_3)
#define TPI_AUDIO_I2S_FS_96000                (BIT_1|BIT_3)
#define TPI_AUDIO_I2S_FS_176400               (BIT_2|BIT_3)
#define TPI_AUDIO_I2S_FS_192000               (BIT_1|BIT_2|BIT_3)

#define TPI_AUDIO_TYPE_REFER              (0x00)
#define TPI_AUDIO_TYPE_PCM                (BIT_0)
#define TPI_AUDIO_TYPE_AC3                 (BIT_1)
#define TPI_AUDIO_TYPE_MPEG1            (BIT_0|BIT_1)
#define TPI_AUDIO_TYPE_MP3                (BIT_2)
#define TPI_AUDIO_TYPE_MPEG2            (BIT_2|BIT_0)
#define TPI_AUDIO_TYPE_AAC                (BIT_2|BIT_1)
#define TPI_AUDIO_TYPE_DTS                (BIT_2|BIT_1|BIT_0)
#define TPI_AUDIO_TYPE_ATRAC            (BIT_3)

// Available only when TPI 0x26[7:6]=10 to select I2S input
//=========================================================
#define TPI_I2S_CHST_0               (0x21*4)
#define TPI_I2S_CHST_1               (0x22*4)
#define TPI_I2S_CHST_2               (0x23*4)
#define TPI_I2S_CHST_3               (0x24*4)
#define TPI_I2S_FS_MASK             (BIT_0|BIT_1|BIT_2|BIT_3)

#define TPI_I2S_CHST_4               (0x25*4)
#define TPI_I2S_SAMPLE_LEN_MASK       (BIT_0|BIT_1|BIT_2|BIT_3)


// Available only when 0x26[7:6]=01
//=================================
#define TPI_SPDIF_HEADER         (0x24*4)
#define TPI_AUDIO_HANDLING    (0x25*4)
#define TPI_SPDIF_AUDIO_DOWN_SAMPLE_MASK (BIT1 | BIT0)
#define TPI_SPDIF_AUDIO_DOWN_SAMPLE_ENABLE (BIT1)

// Audio Configuration Regiaters
//==============================
#define TPI_AUDIO_INTERFACE_REG    (0x26*4)

// Finish this...

#define AUDIO_MUTE_MASK                           (BIT_4)
#define AUDIO_MUTE_NORMAL                      (0x00)
#define AUDIO_MUTE_MUTED                        (0x10)
#define AUDIO_INTERFACE_MASK                  (BIT_6|BIT_7)
#define AUDIO_INTERFACE_DISABLED           (0x00)
#define AUDIO_INTERFACE_I2S                     (BIT_7)
#define AUDIO_INTERFACE_SPDIF                 (BIT_6)
#define AUDIO_INTERFACE_HD                      (BIT_6|BIT_7)
#define AUDIO_LAYOUT_MASK                       (BIT_5)
#define AUDIO_LAYOUT_8CH                         (BIT_5)
#define AUDIO_LAYOUT_2CH                         (0x00)






#define TPI_AUDIO_SAMPLE_CTRL        (0x27*4)
#define TPI_AUDIO_SAMPLE_FREQUENCY         (BIT_3|BIT_4|BIT_5)

#define TPI_CONFIG5v (0x28*4) //added by xueping

#define TPI_SPEAKER_CFG            (0xC7*4)
#define TPI_CODING_TYPE_CHANNEL_COUNT         (0xC4*4)

/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ///

/*\
| | HDCP Implementation
| |
| | HDCP link security logic is implemented in certain transmitters; unique
| |   keys are embedded in each chip as part of the solution. The security
| |   scheme is fully automatic and handled completely by the hardware.
\*/

/// HDCP Query Data Register ============================================== ///

#define TPI_HDCP_QUERY_DATA_REG                                (0x29*4)

#define EXTENDED_LINK_PROTECTION_MASK                (BIT_7)
#define EXTENDED_LINK_PROTECTION_NONE                (0x00)
#define EXTENDED_LINK_PROTECTION_SECURE             (0x80)

#define LOCAL_LINK_PROTECTION_MASK                         (BIT_6)
#define LOCAL_LINK_PROTECTION_NONE                         (0x00)
#define LOCAL_LINK_PROTECTION_SECURE                     (0x40)

#define LINK_STATUS_MASK                   (BIT_5 | BIT_4)
#define LINK_STATUS_NORMAL               (0x00)
#define LINK_STATUS_LINK_LOST           (0x10)
#define LINK_STATUS_RENEGOTIATION_REQ    (0x20)
#define LINK_STATUS_LINK_SUSPENDED           (0x30)

#define HDCP_REPEATER_MASK            (BIT_3)
#define HDCP_REPEATER_NO                 (0x00)
#define HDCP_REPEATER_YES               (0x08)

#define CONNECTOR_TYPE_MASK             (BIT_2 | BIT_0)
#define CONNECTOR_TYPE_DVI                  (0x00)
#define CONNECTOR_TYPE_RSVD             (0x01)
#define CONNECTOR_TYPE_HDMI              (0x04)
#define CONNECTOR_TYPE_FUTURE        (0x05)

#define PROTECTION_TYPE_MASK             (BIT_1)
#define PROTECTION_TYPE_NONE             (0x00)
#define PROTECTION_TYPE_HDCP             (0x02)

/// HDCP Control Data Register ============================================ ///

#define TPI_HDCP_CONTROL_DATA_REG      (0x2A*4)

#define PROTECTION_LEVEL_MASK                 (BIT_0)
#define PROTECTION_LEVEL_MIN                     (0x00)
#define PROTECTION_LEVEL_MAX                    (0x01)

#define KSV_FORWARD_MASK                            (BIT_4)
#define KSV_FORWARD_ENABLE                       (0x10)
#define KSV_FORWARD_DISABLE                      (0x00)

/// HDCP BKSV Registers =================================================== ///

#define TPI_BKSV_1_REG                (0x2B*4)
#define TPI_BKSV_2_REG                (0x2C*4)
#define TPI_BKSV_3_REG                (0x2D*4)
#define TPI_BKSV_4_REG                (0x2E*4)
#define TPI_BKSV_5_REG                (0x2F*4)

/// HDCP Revision Data Register =========================================== ///

#define TPI_HDCP_REVISION_DATA_REG                (0x30*4)

#define HDCP_MAJOR_REVISION_MASK                  (BIT_7 | BIT_6 | BIT_5 | BIT_4)
#define HDCP_MAJOR_REVISION_VALUE                (0x10)

#define HDCP_MINOR_REVISION_MASK                  (BIT_3 | BIT_2 | BIT_1 | BIT_0)
#define HDCP_MINOR_REVISION_VALUE                 (0x02)

/// HDCP KSV and V' Value Data Register =================================== ///

#define TPI_V_PRIME_SELECTOR_REG         (0x31*4)

/// V' Value Readback Registers =========================================== ///

#define TPI_V_PRIME_7_0_REG        (0x32*4)
#define TPI_V_PRIME_15_9_REG      (0x33*4)
#define TPI_V_PRIME_23_16_REG    (0x34*4)
#define TPI_V_PRIME_31_24_REG    (0x35*4)

/// HDCP AKSV Registers =================================================== ///

#define TPI_AKSV_1_REG    (0x36*4)
#define TPI_AKSV_2_REG    (0x37*4)
#define TPI_AKSV_3_REG    (0x38*4)
#define TPI_AKSV_4_REG    (0x39*4)
#define TPI_AKSV_5_REG    (0x3A*4)

#define TPI_DEEP_COLOR_GCP     (0x40*4)

/*\
| | Interrupt Service
| |
| | TPI can be configured to generate an interrupt to the host to notify it of
| |   various events. The host can either poll for activity or use an interrupt
| |   handler routine. TPI generates on a single interrupt (INT) to the host.
\*/

/// Interrupt Enable Register ============================================= ///

#define TPI_INTERRUPT_ENABLE_REG     (0x3C*4)

#define ALL_INTERRUPT_ENABLED    (0xFB)
#define ALL_INTERRUPT_DISABLED    (0x00)

#define HDCP_AUTH_STATUS_CHANGE_EN_MASK        (BIT_7)
#define HDCP_AUTH_STATUS_CHANGE_DISABLE          (0x00)
#define HDCP_AUTH_STATUS_CHANGE_ENABLE           (0x80)

#define HDCP_VPRIME_VALUE_READY_EN_MASK          (BIT_6)
#define HDCP_VPRIME_VALUE_READY_DISABLE            (0x00)
#define HDCP_VPRIME_VALUE_READY_ENABLE             (0x40)

#define HDCP_SECURITY_CHANGE_EN_MASK                  (BIT_5)
#define HDCP_SECURITY_CHANGE_DISABLE                    (0x00)
#define HDCP_SECURITY_CHANGE_ENABLE                     (0x20)

#define AUDIO_ERROR_EVENT_EN_MASK                 (BIT_4)
#define AUDIO_ERROR_EVENT_DISABLE                   (0x00)
#define AUDIO_ERROR_EVENT_ENABLE                    (0x10)

#define CPI_EVENT_NO_RX_SENSE_MASK                (BIT_3)
#define CPI_EVENT_NO_RX_SENSE_DISABLE          (0x00)
#define CPI_EVENT_NO_RX_SENSE_ENABLE           (0x08)

#define RECEIVER_SENSE_EVENT_EN_MASK          (BIT_1)
#define RECEIVER_SENSE_EVENT_DISABLE            (0x00)
#define RECEIVER_SENSE_EVENT_ENABLE             (0x02)

#define HOT_PLUG_EVENT_EN_MASK                         (BIT_0)
#define HOT_PLUG_EVENT_DISABLE                           (0x00)
#define HOT_PLUG_EVENT_ENABLE                            (0x01)

/// Interrupt Status Register ============================================= ///

#define TPI_INTERRUPT_STATUS_REG           (0x3D*4)

#define HDCP_AUTH_STATUS_CHANGE_EVENT_MASK  (BIT_7)
#define HDCP_AUTH_STATUS_CHANGE_EVENT_NO      (0x00)
#define HDCP_AUTH_STATUS_CHANGE_EVENT_YES     (0x80)

#define HDCP_VPRIME_VALUE_READY_EVENT_MASK   (BIT_6)
#define HDCP_VPRIME_VALUE_READY_EVENT_NO       (0x00)
#define HDCP_VPRIME_VALUE_READY_EVENT_YES      (0x40)

#define HDCP_SECURITY_CHANGE_EVENT_MASK          (BIT_5)
#define HDCP_SECURITY_CHANGE_EVENT_NO              (0x00)
#define HDCP_SECURITY_CHANGE_EVENT_YES             (0x20)

#define AUDIO_ERROR_EVENT_MASK                          (BIT_4)
#define AUDIO_ERROR_EVENT_NO                              (0x00)
#define AUDIO_ERROR_EVENT_YES                             (0x10)

#define CPI_EVENT_MASK                               (BIT_3)
#define CPI_EVENT_NO                                   (0x00)
#define CPI_EVENT_YES                                  (0x08)
#define RX_SENSE_MASK                                 (BIT_3)  // This bit is dual purpose depending on the value of 0x3C[3]
#define RX_SENSE_NOT_ATTACHED                 (0x00)
#define RX_SENSE_ATTACHED                         (0x08)

#define HOT_PLUG_PIN_STATE_MASK       (BIT_2)
#define HOT_PLUG_PIN_STATE_LOW        (0x00)
#define HOT_PLUG_PIN_STATE_HIGH       (0x04)

#define RECEIVER_SENSE_EVENT_MASK      (BIT_1)
#define RECEIVER_SENSE_EVENT_NO          (0x00)
#define RECEIVER_SENSE_EVENT_YES         (0x02)

#define HOT_PLUG_EVENT_MASK                      (BIT_0)
#define HOT_PLUG_EVENT_NO                          (0x00)
#define HOT_PLUG_EVENT_YES                         (0x01)

/// KSV FIFO First Status Register ============================================= ///

#define TPI_KSV_FIFO_READY_INT                (0x3E*4)

#define KSV_FIFO_READY_MASK                   (BIT_1)
#define KSV_FIFO_READY_NO                       (0x00)
#define KSV_FIFO_READY_YES                      (0x02)

#define TPI_KSV_FIFO_READY_INT_EN        (0x3F*4)

#define KSV_FIFO_READY_EN_MASK            (BIT_1)
#define KSV_FIFO_READY_DISABLE             (0x00)
#define KSV_FIFO_READY_ENABLE              (0x02)

/// KSV FIFO Last Status Register ============================================= ///

#define TPI_KSV_FIFO_STATUS_REG        (0x41*4)
#define TPI_KSV_FIFO_VALUE_REG          (0x42*4)

#define KSV_FIFO_LAST_MASK           (BIT_7)
#define KSV_FIFO_LAST_NO               (0x00)
#define KSV_FIFO_LAST_YES              (0x80)

#define KSV_FIFO_COUNT_MASK          (BIT_4 | BIT_3 | BIT_2 | BIT_1 | BIT_0)

// Sync Register Configuration and Sync Monitoring Registers
//==========================================================

#define TPI_SYNC_GEN_CTRL                 (0x60*4)
#define TPI_SYNC_POLAR_DETECT         (0x61*4)

// Explicit Sync DE Generator Registers (TPI 0x60[7]=0)
//=====================================================

#define TPI_DE_DLY              (0x62*4)
#define TPI_DE_CTRL            (0x63*4)
#define TPI_DE_TOP              (0x64*4)

#define TPI_RESERVED4        (0x65*4)

#define TPI_DE_CNT_7_0      (0x66*4)
#define TPI_DE_CNT_11_8    (0x67*4)

#define TPI_DE_LIN_7_0       (0x68*4)
#define TPI_DE_LIN_10_8     (0x69*4)

#define TPI_DE_H_RES_7_0   (0x6A*4)
#define TPI_DE_H_RES_10_8 (0x6B*4)

#define TPI_DE_V_RES_7_0    (0x6C*4)
#define TPI_DE_V_RES_10_8  (0x6D*4)

// Embedded Sync Register Set (TPI 0x60[7]=1)
//===========================================

#define TPI_HBIT_TO_HSYNC_7_0     (0x62*4)
#define TPI_HBIT_TO_HSYNC_9_8     (0x63*4)
#define TPI_FIELD_2_OFFSET_7_0     (0x64*4)
#define TPI_FIELD_2_OFFSET_11_8   (0x65*4)
#define TPI_HWIDTH_7_0                  (0x66*4)
#define TPI_HWIDTH_8_9                  (0x67*4)
#define TPI_VBIT_TO_VSYNC             (0x68*4)
#define TPI_VWIDTH                          (0x69*4)

//
// H/W Optimization Control Registers
//
#define TPI_HW_OPT_CTRL_1       (0xB9*4)
#define TPI_HW_OPT_CTRL_2       (0xBA*4)
#define TPI_HW_OPT_CTRL_3       (0xBB*4)

// H/W Optimization Control Register #3 Set
#define DDC_DELAY_BIT9_MASK        (BIT_7)
#define DDC_DELAY_BIT9_NO            (0x00)
#define DDC_DELAY_BIT9_YES           (0x80)
#define RI_CHECK_SKIP_MASK           (BIT_3)
#define RI_CHECK_SKIP_NO               (0x00)
#define RI_CHECK_SKIP_YES              (0x08)


// TPI Enable Register
//====================

#define TPI_ENABLE        (0xC7*4)

// Misc InfoFrames
//================
#define MISC_INFO_FRAMES_CTRL      (0xBF*4)
#define MISC_INFO_FRAMES_TYPE      (0xC0*4)
#define MISC_INFO_FRAMES_VER        (0xC1*4)
#define MISC_INFO_FRAMES_LEN        (0xC2*4)
#define MISC_INFO_FRAMES_CHKSUM (0xC3*4)

#define TPI_LAST_INFOFRAME_DATA_REG     (0xDE*4)

//PHY
#define   HDMI_PHY_OSC_ACLK_CTRL    (0x04*4)
#define   HDMI_PHY_DM_TX_CTRL1    (0x05*4)
#define   HDMI_PHY_HDMI_DRV_CTRL    (0x0E*4)
#define   HDMI_PHY_BIASGEN_CTRL1    (0x0A*4)

#define PHY_DM_TX_TEST_MUX_CTRL_ADDR     (0x0000)
#define PHY_DM_TX_CHIP_ID_ADDR           (0x0004)
#define PHY_PLL_CTRL_ADDR                (0x0008)
#define PHY_OSC_CTRL_ADDR                (0x000C)
#define PHY_OSC_ACLK_CTRL_ADDR           (0x0010)
#define PHY_DM_TX_CTRL1_ADDR             (0x0014)
#define PHY_DM_TX_CTRL2_ADDR             (0x0018)
#define PHY_DM_TX_CTRL3_ADDR             (0x001C)
#define PHY_DM_TX_CTRL4_ADDR             (0x0020)
#define PHY_DM_TX_CTRL5_ADDR             (0x0024)
#define PHY_BIAS_GEN_CTRL1_ADDR          (0x0028)
#define PHY_BIAS_GEN_CTRL2_ADDR          (0x002C)
#define PHY_DM_TX_STAT_ADDR              (0x0030)
#define PHY_HDMI_CTRL_ADDR               (0x0034)
#define PHY_HDMI_DRV_CTRL_ADDR           (0x0038)
#define PHY_MHL_DRV_CTRL_ADDR            (0x003C)
#define PHY_MHL_CTRL1_ADDR               (0x0040)
#define PHY_MHL_CTRL2_ADDR               (0x0044)
#define PHY_MHL_STAT_ADDR                (0x0048)
#define PHY_CBUS_MEAS_RGND_CTRL1_ADDR    (0x004C)
#define PHY_CBUS_MEAS_RGND_CTRL2_ADDR    (0x0050)
#define PHY_CBUS_DRV_CTRL_ADDR           (0x0054)
#define PHY_CBUS_STAT_ADDR               (0x0058)
#define PHY_BIST_CTRL_ADDR               (0x0074)
#define PHY_BIST_STAT_ADDR               (0x0078)
#define PHY_BIST_PATTERN_L_ADDR          (0x0080)
#define PHY_BIST_PATTERN_H_ADDR          (0x0084)
#define PHY_BIST_INSTRUCT_L_ADDR         (0x0088)
#define PHY_BIST_INSTRUCT_H_ADDR         (0x008C)
#define PHY_BIST_CONFIG_L_ADDR           (0x0090)
#define PHY_BIST_CONFIG_H_ADDR           (0x0094)

#define PHY_MHL_30BIT 1
/*
#define   ACLK_MULT_FACTOR_00                (0x0)
#define   ACLK_MULT_FACTOR_01                (0x1)
#define   ACLK_MULT_FACTOR_10                (0x2)
#define   ACLK_MULT_FACTOR_11                (0x3)
*/

typedef enum _hdmi_pix_repeat {
    TPI_PIX_REPEAT_0 = 0x00,
    TPI_PIX_REPEAT_2 = 0x01,
    TPI_PIX_REPEAT_4 = 0x03,
} hdmi_pix_repeat_en;

typedef enum _hdmi_tclk_sel {
    TPI_TCLK_SEL_HALF = 0x00,
    TPI_TCLK_SEL_1X = (0x01<<6),
    TPI_TCLK_SEL_2X = (0x02<<6),
    TPI_TCLK_SEL_4X = (0x03<<6),
} hdmi_clk_sel_en;

typedef enum _hdmi_clk_edge {
    TPI_CLK_EDGE_FALLING = 0,
    TPI_CLK_EDGE_RISING = 1,
} hdmi_clk_edge_en;

typedef enum _hdmi_color_space {
    TPI_COLOR_SPACE_RGB = 0,
    TPI_COLOR_SPACE_YCBCR444 = 1,
    TPI_COLOR_SPACE_YCBCR422 = 2,
    TPI_COLOR_SPACE_RGB_SOLID = 3,
} hdmi_color_space_en;

typedef enum _hdmi_color_depth {
    TPI_COLOR_DEPTH_8BIT  = 0x00,
    TPI_COLOR_DEPTH_10BIT = 0x80,
    TPI_COLOR_DEPTH_12BIT = 0xC0,
    TPI_COLOR_DEPTH_16BIT = 0x40,
} hdmi_color_depth_en;

typedef enum _hdmi_color_stand {
    TPI_COLOR_STANDARD_BT601  = 0x00,
    TPI_COLOR_STANDARD_BT709  = 0x10,
} hdmi_color_standard_en;

typedef enum _hdmi_depth_dither {
    TPI_DEPTH_DITHER_DISABLE  = 0x00,
    TPI_DEPTH_DITHER_ENSABLE  = 0x20,
} hdmi_depth_dither_en;

typedef enum _hdmi_video_range {
    HDMI_VIDEO_RANGE_AUTO    = 0x00,     /* hdmi is auto range */
    HDMI_VIDEO_RANGE_FULL    = 0x04,     /* hdmi is full range */
    HDMI_VIDEO_RANGE_LIMIT   = 0x08,     /* hdmi is limit range */
} hdmi_video_range_en;

typedef enum _hdmi_video_format {
    HDMI_VIDEO_FORMAT_CEA_VIC  = 0x00,
    HDMI_VIDEO_FORMAT_HDMI_VIC = 0x01,
    HDMI_VIDEO_FORMAT_3D       = 0x02,
} hdmi_video_format_en;

typedef struct _hdmi_tpi_video_config_t {
    hdmi_pix_repeat_en     pix_repeat;
    hdmi_clk_sel_en        tclk_sel;
    hdmi_clk_edge_en       clk_edge;
    bool                   use_embedded_sync;
    bool                   use_de;
    hdmi_color_space_en    input_color_space;
    hdmi_color_depth_en    input_color_depth;
    hdmi_video_range_en    input_range;
    hdmi_color_space_en    output_color_space;
    hdmi_color_depth_en    output_color_depth;
    hdmi_video_range_en    output_range;
    hdmi_color_standard_en output_color_standard;
    hdmi_depth_dither_en   output_dither;
    hdmi_s3d_config        s3d_config;
    int                    vic;
    hdmi_video_format_en   video_format;
    int                    aspect_ratio;
    bool                   is_hdmi;
} hdmi_tpi_video_config;

typedef struct _mhl_intr_values_t {
    //mhl intr register value
    u8 mhl_intr4;//mhl intr--zhang test
    u8 mhl_cbus_intr0;
    u8 mhl_cbus_intr1;
    u8 mhl_cbus_status;
    u8 mhl_cbus_set_intr[4];
    u8 mhl_cbus_ddc_abort_intr;
    u8 mhl_cbus_msc_mt_abort_intr;
    u8 mhl_cbus_msc_mr_abort_intr;
    u8 mhl_cbus_pri_rd_data_1st;
    u8 mhl_cbus_write_state[4];
    u8 mhl_cbus_pri_vs_cmd;
    u8 mhl_cbus_pri_vs_data;
} mhl_intr_values;

#define TXL_PAGE_0_HDCP_I_CNT_ADDR  (0x25)

void write_reg(u32 base, u32 idx, u32 val);
u32  read_reg(u32 base, u32 idx);
void dump_reg(void);
//TPI  read and write
void tpi_write_reg(u32 idx, u32 val);
u32 tpi_read_reg(u32 idx);
void tpi_modify_reg(u32 idx, u32 mask, u32 val);
void tpi_write_reg_internal(u32 page, u32 idx, u32 val);
u32 tpi_read_reg_internal(u32 page, u32 idx);
void tpi_modify_reg_internal(u32 page, u32 idx, u32 mask, u32 val);
void tpi_write_reg_block(u32 idx, u32 count, u8* val);

/* resource */
int hw_get_resources(struct platform_device *pdev);
void hw_free_resources(void);

/* iomux */
void hw_set_iomux(char* mode);

/* clock */
void hw_enable_clocks(void);
void hw_disable_clocks(void);

/* edid */
int hw_core_read_edid(u8 *edid, u16 max_length);

/* irq */
int hw_get_irq(void);
void hw_init_irqs(void);
void hw_enable_irqs(bool enable);
int hw_get_hdmi_state(void);
int hw_rx_detect(void);

/* power */
void hw_phy_power_on(void);
void hw_phy_power_off(void);
void hw_core_power_on(void);
void hw_core_power_off(void);

/* config */
void hw_enable(hdmi_config *cfg);
void hw_configure_audio(u32 base_addr, hw_audio_configure *audio_cfg);
int hw_configure_acr(u32 pclk, hdmi_core_fs audio_fs);
int hw_configure_lrfr(hdmi_range);
void hw_phy_configure_aclk_dig(int factory);
void hw_core_mute_audio(bool benabled);
void hw_core_swreset_release(void);
void hw_core_swreset_assert(void);
void hw_core_blank_video(bool blank);

bool hw_support_mhl(void);

bool mhl_is_connected(void);
void hw_enable_tmds(void);
void hw_disable_tmds(void);

int run_all_bist_tests(void);
void hw_reconfig_hdcp(bool hdcp_enable);

#endif
