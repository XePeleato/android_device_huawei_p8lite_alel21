/* Copyright (c) 2008-2011, Hisilicon Tech. Co., Ltd. All rights reserved.
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
 */

#ifndef __K3_HDMI_HW_H__
#define __K3_HDMI_HW_H__

#include <linux/platform_device.h>

#define DTS_COMP_HDMI_NAME "hisilicon,k3hdmi"

/* HDMI IP Core System */
#define HDMI_CORE_SYS_VND_IDL              0x0ul
#define HDMI_CORE_SYS_DEV_IDL              0x8ul
#define HDMI_CORE_SYS_DEV_IDH              0xCul
#define HDMI_CORE_SYS_DEV_REV              0x10ul
#define HDMI_CORE_SYS_SRST                 0x14ul
#define   BIT_TX_SW_RST                     (0x01)
#define   BIT_TX_FIFO_RST                   (0x02)
#define HDMI_CORE_CTRL1                    0x20ul
#define   BIT_TX_PD                         (0x01)
#define   BIT_BSEL24BITS                    (0x04)
#define   BIT_TX_CLOCK_RISING_EDGE          (0x02)
#define HDMI_CORE_SYS_SYS_STAT             0x24ul
#define   BIT_HDMI_PSTABLE                  (0X01)
#define   BIT_HPD_PIN                       (0x02)
#define   BIT_RSEN                          (0x04)
#define HDMI_CORE_SYS_DATA_CTRL            0x34ul
#define   BIT_AUD_MUTE                      (0x02)
#define   BIT_VID_BLANK                     (0x04)
#define HDMI_CORE_SYS_VID_ACEN             0x124ul
#define HDMI_CORE_SYS_VID_MODE             0x128ul
#define HDMI_CORE_SYS_VID_CTRL             0x120ul
#define HDMI_CORE_SYS_INTR_STATE           0x1C0ul
#define   BIT_INTR                          (0x01)
#define HDMI_CORE_SYS_INTR1                0x1C4ul
#define   BIT_INTR1_SOFT                    (0x80)
#define   BIT_INTR1_HPD                     (0x40)
#define   BIT_INTR1_RSEN                    (0x20)
#define   BIT_INTR1_DROP_SAMPLE             (0x10)
#define   BIT_INTR1_BI_PHASE_ERR            (0x08)
#define   BIT_INTR1_RI_128                  (0x04)
#define   BIT_INTR1_OVER_RUN                (0x02)
#define   BIT_INTR1_UNDER_RUN               (0x01)
#define HDMI_CORE_SYS_INTR2                0x1C8ul
#define   BIT_INTR2_BCAP_DONE               (0x80)
#define   BIT_INTR2_SPDIF_PAR               (0x40)
#define   BIT_INTR2_ENC_DIS                 (0x20)
#define   BIT_INTR2_PREAM_ERR               (0x10)
#define   BIT_INTR2_CTS_CHG                 (0x08)
#define   BIT_INTR2_ACR_OVR                 (0x04)
#define   BIT_INTR2_TCLK_STBL               (0x02)
#define   BIT_INTR2_VSYNC_REC               (0x01)
#define HDMI_CORE_SYS_INTR3                0x1CCul
#define   BIT_INTR3_RI_ERR3                 (0x80)
#define   BIT_INTR3_RI_ERR2                 (0x40)
#define   BIT_INTR3_RI_ERR1                 (0x20)
#define   BIT_INTR3_RI_ERR0                 (0x10)
#define   BIT_INTR3_DDC_CMD_DONE            (0x08)
#define   BIT_INTR3_DDC_FIFO_HALF           (0x04)
#define   BIT_INTR3_DDC_FIFO_FULL           (0x02)
#define   BIT_INTR3_DDC_FIFO_EMPTY          (0x01)
#define HDMI_CORE_SYS_INTR4                0x1D0ul
#define   BIT_INTR4_CEC                     (0x08)
#define   BIT_INTR4_DSD_INVALID             (0x01)
#define HDMI_CORE_SYS_INTR1_MASK           0x1D4ul
#define HDMI_CORE_SYS_INTR2_MASK           0x1D8ul
#define HDMI_CORE_SYS_INTR3_MASK           0x1DCul
#define   MASK_AUTO_RI_9134_SPECIFIC        (0xB0)
#define   MASK_AUTO_KSV_READY               (0x80)
#define HDMI_CORE_SYS_INTR4_MASK           0x1E0ul
#define   BIT_INT_Ri_CHECK                  (0x04)

#define HDMI_CORE_SYS_TMDS_CTRL            0x208ul
#define HDMI_CORE_CTRL1_VEN_FOLLOWVSYNC    0x1ul
#define HDMI_CORE_CTRL1_HEN_FOLLOWHSYNC    0x1ul
#define HDMI_CORE_CTRL1_BSEL_24BITBUS      0x1ul
#define HDMI_CORE_CTRL1_EDGE_RISINGEDGE    0x1ul

#define HDMI_CORE_SYS_HDCP_CTRL            0x3cul
#define   BIT_ENC_EN                        (0x01)
#define   BIT_RiREADY                       (0x02)
#define   BIT_RI_STARTED                    (0x01)
#define   BIT_CP_RESET_N                    (0x04)
#define   BIT_AN_STOP                       (0x08)
#define   BIT_RX_REPEATER                   (0x10)
#define   BIT_BKSV_ERROR                    (0x20)
#define   BIT_ENC_ON                        (0x40)

#define HDMI_CORE_SYS_HDCP_BKSV_ADDR       0x40ul
#define HDMI_CORE_SYS_HDCP_AN_ADDR         0x54ul
#define HDMI_CORE_SYS_HDCP_AKSV_ADDR       0x74ul
#define HDMI_CORE_SYS_HDCP_Ri_ADDR         0x88ul
#define HDMI_CORE_SYS_HDCP_RI_STAT         0x98ul
#define HDMI_CORE_SYS_HDCP_RI_CMD_ADDR     0x9cul
#define HDMI_CORE_SYS_HDCP_RI_START        0xA0ul
#define HDMI_CORE_SYS_HDCP_RI_RX_1         0xA4ul
#define HDMI_CORE_SYS_HDCP_RI_RX_2         0xA8ul

#define HDMI_CORE_SYS_DE_DLY               0xC8ul
#define HDMI_CORE_SYS_DE_CTRL              0xCCul
#define   BIT_DE_ENABLED                    (0x40)

#define HDMI_CORE_SYS_DE_TOP               0xD0ul
#define HDMI_CORE_SYS_DE_CNTL              0xD8ul
#define HDMI_CORE_SYS_DE_CNTH              0xDCul
#define HDMI_CORE_SYS_DE_LINL              0xE0ul
#define HDMI_CORE_SYS_DE_LINH_1            0xE4ul
#define HDMI_CORE_SYS_INT_CNTRL            0x1E4ul
#define   BIT_INT_Ri_CHECK                  (0x04)
#define   BIT_INT_HOT_PLUG                  (0x40)
#define   BIT_BIPHASE_ERROR                 (0x08)
#define   BIT_DROP_SAMPLE                   (0x10)
#define   BIT_INT_VSYNC                     (0x01)
#define   BIT_INT_FPIXCHANGE                (0x02)
#define   BIT_INT_KSV_READY                 (0x80)
#define HDMI_CORE_SYS_VID_BLANK1           0x12Cul


/* HDMI IP Core Audio Video */
#define HDMI_CORE_AV_HDMI_CTRL             0xBCul
#define HDMI_CORE_AV_DPD                   0xF4ul
#define HDMI_CORE_AV_PB_CTRL1              0xF8ul
#define HDMI_CORE_AV_PB_CTRL2              0xFCul
#define HDMI_CORE_AV_AVI_TYPE              0x100ul
#define HDMI_CORE_AV_AVI_VERS              0x104ul
#define HDMI_CORE_AV_AVI_LEN               0x108ul
#define HDMI_CORE_AV_AVI_CHSUM             0x10Cul
#define HDMI_CORE_AV_AVI_DBYTE             0x110ul
#define HDMI_CORE_AV_AVI_DBYTE_ELSIZE      0x4ul

/* HDMI DDC E-DID */
#define HDMI_CORE_DDC_CMD                     (0xF3*4)     //  0x3CCul
#define HDMI_CORE_DDC_STATUS               (0xF2*4)     //0x3C8ul
#define   BIT_MDDC_ST_IN_PROGR              (0x10)
#define   BIT_MDDC_ST_I2C_LOW               (0x40)
#define   BIT_MDDC_ST_NO_ACK                (0x20)

#define HDMI_CORE_DDC_ADDR                 (0xED*4)  //0x3B4ul
#define HDMI_CORE_DDC_OFFSET              (0xEF*4)  // 0x3BCul
#define HDMI_CORE_DDC_COUNT1              (0xF0*4) // 0x3C0ul
#define HDMI_CORE_DDC_COUNT2              (0xF1*4)  // 0x3C4ul
#define HDMI_CORE_DDC_DATA                 (0xF4*4)  //0x3D0ul
#define HDMI_CORE_DDC_FIFOCNT             (0xF5*4) // 0x3D4ul
#define HDMI_CORE_DDC_SEGM                   (0xEE*4) //0x3B8ul
#define HDMI_CORE_SYS_SHA_CONTROL       (0xCC*4)  // 0x330ul  /* SHA Control */
#define   BIT_M0_READ_EN                    (0x08)  /* M0 readable (1=external, 0-default=internal) */
#define   BIT_SHA_DONE                      (0x02)  /* The SHA generator picked up the SHA GO START, write a "1" to clear before any new SHA GO START */
#define   BIT_SHA_GO_START                  (0x01)  /* Start the SHA generator */
#define HDMI_CORE_SYS_RI_CMD               0x9Cul
#define   BIT_RI_CMD_BCAP_EN                (0x02)  /* Enable polling of the BCAP "done" bit (KSV FIFO done 0x40[5]). */
#define   BIT_RI_CMD_RI_EN                  (0x01)  /* Enable Ri Check. Need to check the Ri On bit (0x026[0]) for firmware handshaking. */
#define HDMI_CORE_SYS_EPCM                (0xFA*4)  // 0x3E8ul  /* bit5 load ksv */


#define HDMI_CORE_AV_AVI_DBYTE             0x110ul
#define HDMI_CORE_AV_AVI_DBYTE_ELSIZE      0x4ul
#define HDMI_IP_CORE_AV_AVI_DBYTE_NELEMS   15
#define HDMI_CORE_AV_SPD_DBYTE             0x190ul
#define HDMI_CORE_AV_SPD_DBYTE_ELSIZE      0x4ul
#define HDMI_CORE_AV_SPD_DBYTE_NELEMS      27
#define HDMI_CORE_AV_AUDIO_DBYTE           0x210ul
#define HDMI_CORE_AV_AUDIO_DBYTE_ELSIZE    0x4ul
#define HDMI_CORE_AV_AUDIO_DBYTE_NELEMS    10
#define HDMI_CORE_AV_MPEG_DBYTE            0x290ul
#define HDMI_CORE_AV_MPEG_DBYTE_ELSIZE     0x4ul
#define HDMI_CORE_AV_MPEG_DBYTE_NELEMS     27
#define HDMI_CORE_AV_GEN_DBYTE             0x300ul
#define HDMI_CORE_AV_GEN_DBYTE_ELSIZE      0x4ul
#define HDMI_CORE_AV_GEN_DBYTE_NELEMS      31
#define HDMI_CORE_AV_GEN2_DBYTE            0x380ul
#define HDMI_CORE_AV_GEN2_DBYTE_ELSIZE     0x4ul
#define HDMI_CORE_AV_GEN2_DBYTE_NELEMS     31
#define HDMI_CORE_AV_ACR_CTRL              0x4ul
#define HDMI_CORE_AV_FREQ_SVAL             0x8ul
#define HDMI_CORE_AV_N_SVAL1               0xCul
#define HDMI_CORE_AV_N_SVAL2               0x10ul
#define HDMI_CORE_AV_N_SVAL3               0x14ul
#define HDMI_CORE_AV_CTS_SVAL1             0x18ul
#define HDMI_CORE_AV_CTS_SVAL2             0x1Cul
#define HDMI_CORE_AV_CTS_SVAL3             0x20ul
#define HDMI_CORE_AV_CTS_HVAL1             0x24ul
#define HDMI_CORE_AV_CTS_HVAL2             0x28ul
#define HDMI_CORE_AV_CTS_HVAL3             0x2Cul
#define HDMI_CORE_AV_AUD_MODE              0x50ul
#define HDMI_CORE_AV_SPDIF_CTRL            0x54ul
#define HDMI_CORE_AV_HW_SPDIF_FS           0x60ul
#define HDMI_CORE_AV_SWAP_I2S              0x64ul
#define HDMI_CORE_AV_SPDIF_ERTH            0x6Cul
#define HDMI_CORE_AV_I2S_IN_MAP            0x70ul
#define HDMI_CORE_AV_I2S_IN_CTRL           0x74ul
#define HDMI_CORE_AV_I2S_CHST0             0x78ul
#define HDMI_CORE_AV_I2S_CHST1             0x7Cul
#define HDMI_CORE_AV_I2S_CHST2             0x80ul
#define HDMI_CORE_AV_I2S_CHST4             0x84ul
#define HDMI_CORE_AV_I2S_CHST5             0x88ul
#define HDMI_CORE_AV_ASRC                  0x8Cul
#define   BIT_DOWNSAMPLE_RATIO              (0x02)
#define   BIT_DOWNSAMPLE_ENABLE_MASK        (0x01)
#define HDMI_CORE_AV_I2S_IN_LEN            0x90ul
#define   BIT_DEEPCOLOR_EN                  (0x40)
#define   BIT_TXHDMI_MODE                   (0x01)
#define   BIT_EN_AUDIO                      (0x01)
#define   BIT_LAYOUT                        (0x02)
#define   BIT_LAYOUT1                       (0x02)

#define HDMI_CORE_AV_AUDO_TXSTAT           0xC0ul
#define HDMI_CORE_AV_AUD_PAR_BUSCLK_1      0xCCul
#define HDMI_CORE_AV_AUD_PAR_BUSCLK_2      0xD0ul
#define HDMI_CORE_AV_AUD_PAR_BUSCLK_3      0xD4ul
#define HDMI_CORE_AV_TEST_TXCTRL           0xF0ul
#define   BIT_DVI_ENC_BYPASS                (0x08)

#define HDMI_CORE_AV_DPD                   0xF4ul
#define HDMI_CORE_AV_PB_CTRL1              0xF8ul
#define HDMI_CORE_AV_PB_CTRL2              0xFCul
#define   BIT_AVI_REPEAT                    (0x01)
#define   BIT_AVI_ENABLE                    (0x02)
#define   BIT_SPD_REPEAT                    (0x04)
#define   BIT_SPD_ENABLE                    (0x08)
#define   BIT_AUD_REPEAT                    (0x10)
#define   BIT_AUD_ENABLE                    (0x20)
#define   BIT_MPEG_REPEAT                   (0x40)
#define   BIT_MPEG_ENABLE                   (0x80)
#define   BIT_GENERIC_REPEAT                (0x01)
#define   BIT_GENERIC_ENABLE                (0x02)
#define   BIT_CP_REPEAT                     (0x04)
#define   BIT_CP_ENABLE                     (0x08)

#define HDMI_CORE_AV_AVI_TYPE              0x100ul
#define HDMI_CORE_AV_AVI_VERS              0x104ul
#define HDMI_CORE_AV_AVI_LEN               0x108ul
#define HDMI_CORE_AV_AVI_CHSUM             0x10Cul
#define HDMI_CORE_AV_SPD_TYPE              0x180ul
#define HDMI_CORE_AV_SPD_VERS              0x184ul
#define HDMI_CORE_AV_SPD_LEN               0x188ul
#define HDMI_CORE_AV_SPD_CHSUM             0x18Cul
#define HDMI_CORE_AV_AUDIO_TYPE            0x200ul
#define HDMI_CORE_AV_AUDIO_VERS            0x204ul
#define HDMI_CORE_AV_AUDIO_LEN             0x208ul
#define HDMI_CORE_AV_AUDIO_CHSUM           0x20Cul
#define HDMI_CORE_AV_MPEG_TYPE             0x280ul
#define HDMI_CORE_AV_MPEG_VERS             0x284ul
#define HDMI_CORE_AV_MPEG_LEN              0x288ul
#define HDMI_CORE_AV_MPEG_CHSUM            0x28Cul
#define HDMI_CORE_AV_CP_BYTE1              0x37Cul
#define   BIT_CP_AVI_MUTE_SET               (0x01)
#define   BIT_CP_AVI_MUTE_CLEAR             (0x10)

#define HDMI_CORE_AV_CEC_ADDR_ID           0x3FCul

#define HDMI_CORE_SYS_SYS_STAT_HPD         0x02

#define HDMI_IP_CORE_SYSTEM_INTR2_BCAP     0x80
#define HDMI_IP_CORE_SYSTEM_INTR3_RI_ERR   0xF0

/*hdmi phy */
#define HDMI_BEST_ACLK_DIG                 135000  //as KHz
#define HDMI_MAX_ACLK_DIG                  165000  //as KHz
#define HDMI_MIN_ACLK_DIG                  85000  //as KHz

#define HDMI_PHY_TDMS_CTL1                 0x0

#define HDMI_PHY_TDMS_CTL2                 0x04
#define HDMI_PHY_TDMS_CTL3                 0x08
#define HDMI_PHY_BIST_CNTL                 0x0C
#define HDMI_PHY_BIST_INSTRL               0x18
#define HDMI_PHY_TDMS_CNTL9                0x20

#define   ACLK_MULT_FACTOR_1                (0x0)
#define   ACLK_MULT_FACTOR_2                (0x4)
#define   ACLK_MULT_FACTOR_3                (0x2)
#define   ACLK_MULT_FACTOR_4                (0x5)
#define   ACLK_MULT_FACTOR_5                (0x3)
#define   ACLK_MULT_FACTOR_6                (0x6)
#define   ACLK_MULT_FACTOR_10               (0x7)

#define   BIT_ACLK_COUNT0                   (0x5)
#define   BIT_ACLK_COUNT1                   (0x6)
#define   BIT_ACLK_COUNT2                   (0x5)

#define FLD_MASK(start, end)    (((1 << ((start) - (end) + 1)) - 1) << (end))
#define FLD_VAL(val, start, end) (((val) << (end)) & FLD_MASK(start, end))
#define FLD_GET(val, start, end) (((val) & FLD_MASK(start, end)) >> (end))
#define FLD_MOD(orig, val, start, end) \
    (((orig) & ~FLD_MASK(start, end)) | FLD_VAL(val, start, end))

#define REG_FLD_MOD(base, idx, val, start, end) \
    write_reg(base, idx, \
        FLD_MOD(read_reg(base, idx), val, start, end))

#define RD_REG_32(COMP, REG)            read_reg(COMP, REG)
#define WR_REG_32(COMP, REG, VAL)       write_reg(COMP, REG, (u32)(VAL))

#define BITS_32(in_NbBits) \
        ((((u32)1 << in_NbBits) - 1) | ((u32)1 << in_NbBits))

#define BITFIELD(in_UpBit, in_LowBit) \
        (BITS_32(in_UpBit) & ~((BITS_32(in_LowBit)) >> 1))

#define HDMI_CONNECT            0x01
#define HDMI_DISCONNECT         0x02
#define HDMI_INT_CEC            0x04
#define HDMI_FIRST_HPD          0x08
#define HDMI_BCAP               0x40
#define HDMI_RI_ERR             0x80
#define HDMI_RI_128_ERR         0x100

#define HDMI_EVENT_POWEROFF     0x00
#define HDMI_EVENT_POWERPHYOFF  0x01
#define HDMI_EVENT_POWERPHYON   0x02
#define HDMI_EVENT_POWERON      0x03

#define HDMI_AV_REG_OFFSET      0x400
#define HDMI_PHY_REG_OFFSET     0x1800
#define HDMI_CEC_REG_OFFSET     0x800

#define HDMI_CLK_NAME       "clk_hdmi"
#define HDMI_M_CLK_NAME     "clk_hdmi_m"
#define HDMI_ASP_CLK_NAME   "clk_asphdmi"

#define HDMI_IRQ_NAME       "irq_hdmi"
#define HDMI_REG_NAME       "reg_hdmi"
#define HDMI_IOMUX_NAME     "block_hdmi"

#define HDMI_LDI1_CLK_NAME      "clk_ldi1"
#define HDMI_EDC1_CLK_NAME      "clk_edc1"
#define HDMI_EDC1_VCC_NAME      "vcc_edc1"
#define HDMI_APB_CLK_NAME       "clk_pciphy"
#define HDMI_CHARGE_PUMP_NAME   "hdmi-chg-pump"
#define HDMI_CEC_CLK_NAME       "clk_cec"

typedef enum _hdmi_core_inputbus_width {
    HDMI_INPUT_8BIT  = 0,   /* input bus width with 8 bit */
    HDMI_INPUT_10BIT = 1,   /* input bus width with 10 bit */
    HDMI_INPUT_12BIT = 2    /* input bus width with 12 bit */
} hdmi_core_inputbus_width;

typedef enum _hdmi_core_dither_trunc {
    HDMI_OUTPUT_TRUNCATION_8BIT  = 0,   /* output truncation with 8 bit */
    HDMI_OUTPUT_TRUNCATION_10BIT = 1,   /* output truncation with 10 bit */
    HDMI_OUTPUT_TRUNCATION_12BIT = 2,   /* output truncation with 12 bit */
    HDMI_OUTPUT_DITHER_8BIT      = 3,   /* output dither with 8 bit */
    HDMI_OUTPUT_DITHER_10BIT     = 4,   /* output dither with 10 bit */
    HDMI_OUTPUT_DITHER_12BIT     = 5    /* output dither with 12 bit */
} hdmi_core_dither_trunc;

typedef enum _hdmi_core_deep_color_ed {
    HDMI_DEEP_COLOR_PACKECT_DISABLE = 0,    /* deep color packect is disable */
    HDMI_DEEP_COLOR_PACKECT_ENABLE  = 1     /* deep color packect is enable */
} hdmi_core_deep_color_ed;

typedef enum _hdmi_core_packet_mode {
    HDMI_PACKET_MODE_RESERVED_VALUE  = 0,   /* packet mode reserved value */
    HDMI_PACKET_MODE_24BIT_PER_PIXEL = 4,   /* packet mode 24 bit per pixel */
    HDMI_PACKET_MODE_30BIT_PER_PIXEL = 5,   /* packet mode 30 bit per pixel */
    HDMI_PACKET_MODE_36BIT_PER_PIXEL = 6,   /* packet mode 36 bit per pixel */
    HDMI_PACKET_MODE_48BIT_PER_PIXEL = 7    /* packet mode 48 bit per pixel */
} hdmi_core_packet_mode;

typedef enum _hdmi_core_hdmi_dvi {
    HDMI_DVI     = 0,   /* hdmi mode -- dvi mode */
    HDMI_HDMI    = 1    /* hdmi mode -- hdmi mode */
} hdmi_core_hdmi_dvi;

typedef enum _hdmi_core_tclkselclkmult {
    FPLL05IDCK  = 0,    /* FPLL is 0.5*IDCK */
    FPLL10IDCK  = 1,    /* FPLL is 1.0*IDCK */
    FPLL20IDCK  = 2,    /* FPLL is 2.0*IDCK */
    FPLL40IDCK  = 3     /* FPLL is 4.0*IDCK */
} hdmi_core_tclkselclkmult;

typedef struct _hdmi_core_video_config_t {
    hdmi_core_inputbus_width input_bus_wide;              /* input bus wide */
    hdmi_core_dither_trunc   output_dither_truncation;    /* output dither truncation */
    hdmi_core_deep_color_ed  deep_color_packet_enabled;   /* deep color packet */
    hdmi_core_packet_mode    packet_mode;                 /* packet mode */
    hdmi_core_hdmi_dvi       hdmi_dvi;                    /* hdmi or dvi mode */
    hdmi_core_tclkselclkmult tclk_sel_clk_mult;
} hdmi_core_video_config;

typedef enum _hdmi_core_fs {
    FS_32000  = 0x3,        /* audio fs is    32 kHz */
    FS_44100  = 0x0,        /* audio fs is  44.1 kHz */
    FS_48000  = 0x2,        /* audio fs is    48 kHz */
    FS_88200  = 0x8,        /* audio fs is  88.2 kHz */
    FS_96000  = 0xA,        /* audio fs is    96 kHz */
    FS_176400 = 0xC,        /* audio fs is 176.4 kHz */
    FS_192000 = 0xE,        /* audio fs is   192 kHz */
    FS_NOT_INDICATED = 0x1  /* audio fs is indicated */
} hdmi_core_fs;

typedef enum _hdmi_core_layout {
    LAYOUT_2CH = 0,     /* audio is 2 channel */
    LAYOUT_8CH = 1      /* audio is 8 channel */
} hdmi_core_layout;

typedef enum _hdmi_core_cts_mode {
    CTS_MODE_HW = 0,    /* cts make reference to hardware */
    CTS_MODE_SW = 1     /* cts make reference to software */
} hdmi_core_cts_mode;

typedef enum _hdmi_core_packet_ctrl {
    PACKET_ENABLE    = 1,   /* packet enable */
    PACKET_DISABLE   = 0,   /* packet disable */
    PACKET_REPEAT_ON  = 1,  /* packet repeat is on */
    PACKET_REPEAT_OFF = 0   /* packet repeat is off */
} hdmi_core_packet_ctrl;

/* INFOFRAME_AVI_ definations */
typedef enum _hdmi_core_infoframe {
    INFOFRAME_AVI_DB1Y_RGB      = 0,
    INFOFRAME_AVI_DB1Y_YUV422   = 1,
    INFOFRAME_AVI_DB1Y_YUV444   = 2,
    INFOFRAME_AVI_DB1A_ACTIVE_FORMAT_OFF = 0,
    INFOFRAME_AVI_DB1A_ACTIVE_FORMAT_ON  = 1,
    INFOFRAME_AVI_DB1B_NO       = 0,
    INFOFRAME_AVI_DB1B_VERT     = 1,
    INFOFRAME_AVI_DB1B_HORI     = 2,
    INFOFRAME_AVI_DB1B_VERTHORI = 3,
    INFOFRAME_AVI_DB1S_0        = 0,
    INFOFRAME_AVI_DB1S_1        = 1,
    INFOFRAME_AVI_DB1S_2        = 2,
    INFOFRAME_AVI_DB2C_NO       = 0,
    INFOFRAME_AVI_DB2C_ITU601   = 1,
    INFOFRAME_AVI_DB2C_ITU709   = 2,
    INFOFRAME_AVI_DB2C_EC_EXTENDED  = 3,
    INFOFRAME_AVI_DB2M_NO       = 0,
    INFOFRAME_AVI_DB2M_43       = 1,
    INFOFRAME_AVI_DB2M_169      = 2,
    INFOFRAME_AVI_DB2R_SAME     = 8,
    INFOFRAME_AVI_DB2R_43       = 9,
    INFOFRAME_AVI_DB2R_169      = 10,
    INFOFRAME_AVI_DB2R_149      = 11,
    INFOFRAME_AVI_DB3ITC_NO     = 0,
    INFOFRAME_AVI_DB3ITC_YES    = 1,
    INFOFRAME_AVI_DB3EC_XVYUV601    = 0,
    INFOFRAME_AVI_DB3EC_XVYUV709    = 1,
    INFOFRAME_AVI_DB3Q_DEFAULT  = 0,
    INFOFRAME_AVI_DB3Q_LR       = 1,
    INFOFRAME_AVI_DB3Q_FR       = 2,
    INFOFRAME_AVI_DB3SC_NO      = 0,
    INFOFRAME_AVI_DB3SC_HORI    = 1,
    INFOFRAME_AVI_DB3SC_VERT    = 2,
    INFOFRAME_AVI_DB3SC_HORIVERT    = 3,
    INFOFRAME_AVI_DB5PR_NO      = 0,
    INFOFRAME_AVI_DB5PR_2       = 1,
    INFOFRAME_AVI_DB5PR_3       = 2,
    INFOFRAME_AVI_DB5PR_4       = 3,
    INFOFRAME_AVI_DB5PR_5       = 4,
    INFOFRAME_AVI_DB5PR_6       = 5,
    INFOFRAME_AVI_DB5PR_7       = 6,
    INFOFRAME_AVI_DB5PR_8       = 7,
    INFOFRAME_AVI_DB5PR_9       = 8,
    INFOFRAME_AVI_DB5PR_10      = 9
} hdmi_core_infoframe;

typedef struct _hdmi_core_infoframe_avi {
    u8  db1y_rgb_yuv422_yuv444;             /* yuv format */
    u8  db1a_active_format_off_on;          /* active information present */
    u8  db1b_no_vert_hori_verthori;         /* bar info data valid */
    u8  db1s_0_1_2;                         /* scan information */
    u8  db2c_no_itu601_itu709_extented;     /* colorimetry */
    u8  db2m_no_43_169;                     /* picture aspect ratio */
    u8  db2r_same_43_169_149;               /* active format aspect ratio */
    u8  db3itc_no_yes;                      /* IT content */
    u8  db3ec_xvyuv601_xvyuv709;            /* extended colorimetry */
    u8  db3q_default_lr_fr;                 /* quantization range */
    u8  db3sc_no_hori_vert_horivert;        /* Non-uniform picture scaling */
    u8  db4vic_videocode;                   /* video format identification code */
    u8  db5pr_no_2_3_4_5_6_7_8_9_10;        /* pixel repetition factor */
    u16 db6_7_lineendoftop;                 /* line end of top */
    u16 db8_9_linestartofbottom;            /* line start of bottom */
    u16 db10_11_pixelendofleft;             /* pixel end of left */
    u16 db12_13_pixelstartofright;          /* pixel start of right */
} hdmi_core_infoframe_avi;

typedef struct _hdmi_core_packet_enable_repeat {
    u32 mpeg_info_frame_ed;                 /* mpeg info repeat state */
    u32 mpeg_info_frame_repeat;             /* mpeg info repeat on/off */

    u32 audio_packet_ed;                    /* audio info repeat state */
    u32 audio_packet_repeat;                /* audio info repeat on/off */

    u32 spd_info_frame_ed;                  /* spd info repeat state */
    u32 spd_info_frame_repeat;              /* spd info repeat on/off */

    u32 avi_info_frame_ed;                  /* avi info repeat state */
    u32 avi_info_frame_repeat;              /* avi info repeat on/off */

    u32 general_control_packet_ed;          /* general info repeat state */
    u32 general_control_packet_repeat;      /* general info repeat on/off */

    u32 generic_packet_ed;                  /* generic info repeat state */
    u32 generic_packet_repeat;              /* generic info repeat on/off */
} hdmi_core_packet_enable_repeat;

typedef enum _hdmi_stereo_channel {
    HDMI_STEREO_NOCHANNEL     = 0,  /* 0 stereo channel */
    HDMI_STEREO_ONECHANNELS   = 1,  /* 1 stereo channel */
    HDMI_STEREO_TWOCHANNELS   = 2,  /* 2 stereo channel */
    HDMI_STEREO_THREECHANNELS = 3,  /* 3 stereo channel */
    HDMI_STEREO_FOURCHANNELS  = 4   /* 4 stereo channel */
} hdmi_stereo_channel;

/* cea code */
typedef enum _hdmi_cea_code {
    HDMI_CEA_CODE_00 = 0x0,
    HDMI_CEA_CODE_01 = 0x1,
    HDMI_CEA_CODE_02 = 0x2,
    HDMI_CEA_CODE_03 = 0x3,
    HDMI_CEA_CODE_04 = 0x4,
    HDMI_CEA_CODE_05 = 0x5,
    HDMI_CEA_CODE_06 = 0x6,
    HDMI_CEA_CODE_07 = 0x7,
    HDMI_CEA_CODE_08 = 0x8,
    HDMI_CEA_CODE_09 = 0x9,
    HDMI_CEA_CODE_0A = 0xA,
    HDMI_CEA_CODE_0B = 0xB,
    HDMI_CEA_CODE_0C = 0xC,
    HDMI_CEA_CODE_0D = 0xD,
    HDMI_CEA_CODE_0E = 0xE,
    HDMI_CEA_CODE_0F = 0xF,
    HDMI_CEA_CODE_10 = 0x10,
    HDMI_CEA_CODE_11 = 0x11,
    HDMI_CEA_CODE_12 = 0x12,
    HDMI_CEA_CODE_13 = 0x13,
    HDMI_CEA_CODE_14 = 0x14,
    HDMI_CEA_CODE_15 = 0x15,
    HDMI_CEA_CODE_16 = 0x16,
    HDMI_CEA_CODE_17 = 0x17,
    HDMI_CEA_CODE_18 = 0x18,
    HDMI_CEA_CODE_19 = 0x19,
    HDMI_CEA_CODE_1A = 0x1A,
    HDMI_CEA_CODE_1B = 0x1B,
    HDMI_CEA_CODE_1C = 0x1C,
    HDMI_CEA_CODE_1D = 0x1D,
    HDMI_CEA_CODE_1E = 0x1E,
    HDMI_CEA_CODE_1F = 0x1F,
    HDMI_CEA_CODE_20 = 0x20,
    HDMI_CEA_CODE_21 = 0x21,
    HDMI_CEA_CODE_22 = 0x22,
    HDMI_CEA_CODE_23 = 0x23,
    HDMI_CEA_CODE_24 = 0x24,
    HDMI_CEA_CODE_25 = 0x25,
    HDMI_CEA_CODE_26 = 0x26
} hdmi_cea_code;

typedef enum _hdmi_iec_format {
    HDMI_AUDIO_FORMAT_LPCM = 0, /* audio format is Linear PCM */
    HDMI_AUDIO_FORMAT_IEC  = 1  /* audio format is IEC PCM */
} hdmi_iec_format;

typedef enum _hdmi_audio_justify {
    HDMI_AUDIO_JUSTIFY_LEFT  = 0,   /* audio justify is left */
    HDMI_AUDIO_JUSTIFY_RIGHT = 1    /* audio justify is right */
} hdmi_audio_justify;

typedef enum _hdmi_sample_order {
    HDMI_SAMPLE_RIGHT_FIRST = 0,    /* sample right channel first */
    HDMI_SAMPLE_LEFT_FIRST  = 1     /* sample left channel first */
} hdmi_sample_order;

typedef enum _hdmi_sample_perword {
    HDMI_ONEWORD_ONE_SAMPLE  = 0,   /* one sample in one word */
    HDMI_ONEWORD_TWO_SAMPLES = 1    /* two samples in one word */
} hdmi_sample_perword;

typedef enum _hdmi_sample_size {
    HDMI_SAMPLE_16BITS = 0,         /* sample size is 16 bits */
    HDMI_SAMPLE_24BITS = 1          /* sample size is 24 bits */
} hdmi_sample_size;

typedef enum _hdmi_block_start_end {
    HDMI_BLOCK_STARTEND_ON  = 0,    /* block start end is on */
    HDMI_BLOCK_STARTEND_OFF = 1     /* block start end is off */
} hdmi_block_start_end;

typedef struct _hdmi_audio_format {
    hdmi_stereo_channel    stereo_channel_enable;   /* stereo channel enable */
    hdmi_cea_code          audio_channel_location;  /* audio channel location */
    hdmi_iec_format        iec;                     /* iec format */
    hdmi_audio_justify     justify;                 /* audio justify */
    hdmi_sample_order      left_before;             /* sample order */
    hdmi_sample_perword    sample_number;           /* sample perword */
    hdmi_sample_size       sample_size;             /* sample size */
    hdmi_block_start_end   block_start_end;         /* block start end */
} hdmi_audio_format;

typedef enum _hdmi_packing_mode {
    HDMI_PACK_10b_RGB_YUV444        = 0,
    HDMI_PACK_24b_RGB_YUV444_YUV422 = 1,
    HDMI_PACK_ALREADYPACKED         = 7
} hdmi_packing_mode;

typedef enum _hdmi_deep_color_mode {
    HDMI_DEEP_COLOR_24BIT = 0,  /* deep color is 24 bit */
    HDMI_DEEP_COLOR_30BIT = 1,  /* deep color is 30 bit */
    HDMI_DEEP_COLOR_36BIT = 2,  /* deep color is 36 bit */
    HDMI_DEEP_COLOR_48BIT = 3   /* deep color is 48 bit */
} hdmi_deep_color_mode;

typedef enum _hdmi_range {
    HDMI_LIMITED_RANGE = 0,     /* hdmi is limited range */
    HDMI_FULL_RANGE    = 1,     /* hdmi is full range */
} hdmi_range;

typedef struct _hdmi_s3d_config {
    int structure;          /* 3d structure */
    int s3d_ext_data;       /* 3d extend data */
} hdmi_s3d_config;

typedef struct _hdmi_config {
    u16 ppl;            /* pixel per line */
    u16 lpp;            /* line per panel */
    u32 pixel_clock;    /* pixel clock */
    u16 hsw;            /* Horizontal synchronization pulse width */
    u16 hfp;            /* Horizontal front porch */
    u16 hbp;            /* Horizontal back porch */
    u16 vsw;            /* Vertical synchronization pulse width */
    u16 vfp;            /* Vertical front porch */
    u16 vbp;            /* Vertical back porch */
    u16 interlace;      /* whether resolution is interlace */
    u16 h_pol;          /* Horizontal polarity */
    u16 v_pol;          /* Vertical polarity */
    hdmi_core_hdmi_dvi hdmi_dvi;    /* hdmi mode */
    u16 video_format;               /* timing code */
    u16 deep_color;                 /* deep color */
    u16 s3d_structure;              /* Frame Structure for the S3D Frame */
    u16 subsamp_pos;                /* Subsampling used in Vendor Specific Infoframe */
    int vsi_enabled;                /* Vender Specific InfoFrame enabled/disabled */
    bool supports_ai;               /* whether supports ai */
    bool has_vcdb;                  /*whether has video capability data block*/
} hdmi_config;

typedef enum _hdmi_core_if_fs {
    IF_FS_NO     = 0x0,     /* audio fs is indicated */
    IF_FS_32000  = 0x1,     /* audio fs is    32 kHz */
    IF_FS_44100  = 0x2,     /* audio fs is  44.1 kHz */
    IF_FS_48000  = 0x3,     /* audio fs is    48 kHz */
    IF_FS_88200  = 0x4,     /* audio fs is  88.2 kHz */
    IF_FS_96000  = 0x5,     /* audio fs is    96 kHz */
    IF_FS_176400 = 0x6,     /* audio fs is 176.4 kHz */
    IF_FS_192000 = 0x7      /* audio fs is   192 kHz */
} hdmi_core_if_fs;

typedef enum _hdmi_core_if_sample_size {
    IF_NO_PER_SAMPLE    = 0x0,  /* no sample size */
    IF_16BIT_PER_SAMPLE = 0x1,  /* 16 bit per sample size */
    IF_20BIT_PER_SAMPLE = 0x2,  /* 20 bit per sample size */
    IF_24BIT_PER_SAMPLE = 0x3   /* 24 bit per sample size */
} hdmi_core_if_sample_size;

typedef enum _hdmi_i2s_chst_audio_max_word_length {
    I2S_CHST_WORD_MAX_20 = 0,   /* max word length is 20 */
    I2S_CHST_WORD_MAX_24 = 1,   /* max word length is 24 */
} hdmi_i2s_chst_audio_max_word_length;

/* The word length depends on how the max word length is set.
 * Therefore, some values are duplicated. */
typedef enum _hdmi_i2s_chst_audio_word_length {
    I2S_CHST_WORD_NOT_SPECIFIED = 0x0,
    I2S_CHST_WORD_16_BITS       = 0x1,
    I2S_CHST_WORD_17_BITS       = 0x6,
    I2S_CHST_WORD_18_BITS       = 0x2,
    I2S_CHST_WORD_19_BITS       = 0x4,
    I2S_CHST_WORD_20_BITS_20MAX = 0x5,
    I2S_CHST_WORD_20_BITS_24MAX = 0x1,
    I2S_CHST_WORD_21_BITS       = 0x6,
    I2S_CHST_WORD_22_BITS       = 0x2,
    I2S_CHST_WORD_23_BITS       = 0x4,
    I2S_CHST_WORD_24_BITS       = 0x5,
} hdmi_i2s_chst_audio_word_length;

typedef enum _hdmi_i2s_in_length {
    I2S_IN_LENGTH_NA = 0x0,
    I2S_IN_LENGTH_16 = 0x2,
    I2S_IN_LENGTH_17 = 0xC,
    I2S_IN_LENGTH_18 = 0x4,
    I2S_IN_LENGTH_19 = 0x8,
    I2S_IN_LENGTH_20 = 0xA,
    I2S_IN_LENGTH_21 = 0xD,
    I2S_IN_LENGTH_22 = 0x5,
    I2S_IN_LENGTH_23 = 0x9,
    I2S_IN_LENGTH_24 = 0xb,
} hdmi_i2s_in_length;

typedef enum _hdmi_core_av_csc {
    RGB        = 0x0,   /* color space is rgb */
    RGB_TO_YUV = 0x1,   /* color space is from rgb to yuv */
    YUV_TO_RGB = 0x2    /* color space is from yuv to rgb */
} hdmi_core_av_csc;

typedef struct _hw_audio_configure {
    hdmi_core_fs   fs;              /* Fs Sampling Frequency */
    u32  n;                         /* N Value for audio clock regeneration method */
    u32  cts;                       /* CTS Value for audio clock regeneration method */
    u32  aud_par_busclk;            /* audio bus clock */
    bool bsio;                      /* sio input */
    u32  audiotype;                 /* compressed */
    hdmi_core_layout   layout;      /* Audio packet header layout indicator; */
                                    /* 0x0 = Layout 0 (2-channel); 0x1 = Layout 1 (up to 8 channels)*/
    hdmi_core_cts_mode cts_mode;    /* CTS Source Select (1:SW, 0:HW) */
    hdmi_core_if_fs    if_fs;       /* Sampling Frequency */
    u32  if_channel_number;         /* audio channel number */
    hdmi_core_if_sample_size   if_sample_size;                      /* sample size */
    hdmi_cea_code              if_audio_channel_location;           /* Channel/Speaker allocation */
    hdmi_i2s_chst_audio_max_word_length i2schst_max_word_length;    /* Maximum audio sample word length */
    hdmi_i2s_chst_audio_word_length i2schst_word_length;            /* Audio sample word length */
    hdmi_i2s_in_length i2s_in_bit_length;                           /* Number of valid bits in the input I2S stream. */
    hdmi_audio_justify i2s_justify;                                 /* audio justify */
} hw_audio_configure;

/* HDMI power states */
typedef enum _hdmi_power_state {
    HDMI_POWER_OFF  = 0,
    HDMI_POWER_MIN  = 1,    /* minimum power for HPD detect */
    HDMI_POWER_FULL = 2,    /* full power */
} hdmi_power_state;

/* HDMI states */
typedef enum _hdmi_display_state {
    HDMI_DISPLAY_DISABLED = 0,  /* hdmi display disable state */
    HDMI_DISPLAY_ACTIVE,        /* hdmi display active state */
    HDMI_DISPLAY_SUSPENDED,     /* hdmi display suspended state */
    HDMI_DISPLAY_TRANSITION,    /* hdmi display transition state */
} hdmi_display_state;

typedef enum _hdmi_clock {
    HDMI_CLK     = 1 << 0,
    HDMI_CLK_EDC1= 1 << 3,
    HDMI_CLK_LDI1= 1 << 4,
    HDMI_CLK_CEC = 1 << 5,
} hdmi_clock;

struct hdmi_pin {
    struct device *dev;
    struct pinctrl *pin;
};
/* hdmi hardware resource */
typedef struct _hdmi_hw_res {
    /* reg addr */
    u32 base_core;              /* base address of hdmi ip core */
    u32 base_core_av;           /* base address of hdmi ip core audio and video */
    u32 base_phy;               /* base address of hdmi ip phy */
    u32 base_cec;               /* base address of hdmi ip cec */
    u32 base_crgctrl;
    u32 base_pmctrl;
    u32 base_ldi1;
    u32 base_dss;
    /* clk */
    struct clk *clk_edc0;       /* clock of edc0 */
    struct clk *clk_ldi1;       /* clock of ldi1 */
    struct clk *clk_pclk_hdmi;  /* clock of pclk hdmi */
    struct clk *clk_cec;        /* clock of cec */
    struct clk *clk_hdmiefc;    /* clock of hdmiefc */
    struct clk *clk_ppll2;
    struct clk *clk_muxldi1;

    struct regulator *sdp_vcc;  /* regulator of edc1*/
    struct regulator *boost_vcc;
    struct hdmi_pin *h_pin;

    int irq;
} hdmi_hw_res;

extern hdmi_hw_res hw_res;
#define HDMI_CORE_SYS        hw_res.base_core//0xE852C000             /* HDMI IP Core System */
#define HDMI_CORE_AV         hw_res.base_core_av  /* HDMI IP Core audio and video */
#define HDMI_PHY_BASE        hw_res.base_phy /* HDMI PHY */
#define HDMI_CEC_BASE        hw_res.base_cec /* HDMI CEC */

#endif
