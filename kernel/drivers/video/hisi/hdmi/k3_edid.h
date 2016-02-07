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

#ifndef __K3_EDID_H__
#define __K3_EDID_H__

#define K3_FB_MAX_WIDTH  1600
#define K3_FB_MAX_HEIGHT 2560

#define HDMI_DEFAULT_PCM_PLAY        1 //release: 1

/* HDMI EDID Extension Data Block Tags  */
#define HDMI_EDID_EX_DATABLOCK_TAG_MASK         0xE0  /* tag mask of data block in cea */
#define HDMI_EDID_EX_DATABLOCK_LEN_MASK         0x1F  /* length of data block in cea */
#define HDMI_EDID_EX_SUPPORTS_AI_MASK           0x80  /* bit mask of support ai */
#define HDMI_EDID_EX_DATABLOCK_AUDIO            0x20  /* tag of audio block in cea */
#define HDMI_EDID_EX_DATABLOCK_VIDEO            0x40  /* tag of video block in cea */
#define HDMI_EDID_EX_DATABLOCK_VENDOR           0x60  /* tag of vendor block in cea */
#define HDMI_EDID_EX_DATABLOCK_SPEAKERS         0x80  /* tag of speakers block in cea */

#define EDID_TIMING_DESCRIPTOR_SIZE             0x12  /* size of timing descriptor */
#define EDID_DESCRIPTOR_BLOCK0_ADDRESS          0x36  /* address of first 18 byte data block */
#define EDID_DESCRIPTOR_BLOCK1_ADDRESS          0x80  /* address of cea */
#define EDID_SIZE_BLOCK0_TIMING_DESCRIPTOR      4     /* size of timing descriptor in edid block0 */
#define EDID_SIZE_BLOCK1_TIMING_DESCRIPTOR      4     /* size of timing descriptor in edid block1 */

/* EDID Detailed Timing    Info 0 begin offset */
#define HDMI_EDID_DETAILED_TIMING_OFFSET        0x36  /* address of first detailed timing */

#define HDMI_EDID_PIX_CLK_OFFSET            0    /* offset of pixel clock in detailed timing */
#define HDMI_EDID_H_ACTIVE_OFFSET           2    /* offset of horizontal addressable video in pixels */
#define HDMI_EDID_H_BLANKING_OFFSET         3    /* offset of horizontal blanking in pixels */
#define HDMI_EDID_V_ACTIVE_OFFSET           5    /* offset of vertical addressable video in lines */
#define HDMI_EDID_V_BLANKING_OFFSET         6    /* offset of vertical blanking in lines */
#define HDMI_EDID_H_SYNC_OFFSET             8    /* offset of Horizontal Front Porch in pixels */
#define HDMI_EDID_H_SYNC_PW_OFFSET          9    /* offset of Horizontal Sync Pulse Width in pixels */
#define HDMI_EDID_V_SYNC_OFFSET             10   /* offset of Vertical Front Porch in Lines */
#define HDMI_EDID_V_SYNC_PW_OFFSET          11   /* offset of Vertical Sync Pulse Width in lines */
#define HDMI_EDID_H_IMAGE_SIZE_OFFSET       12   /* offset of Horizontal Addressable Video Image Size in mm */
#define HDMI_EDID_V_IMAGE_SIZE_OFFSET       13   /* offset of Vertical Addressable Video Image Size in mm */
#define HDMI_EDID_H_BORDER_OFFSET           15   /* offset of Right Horizontal Border or Left Horizontal Border in pixels */
#define HDMI_EDID_V_BORDER_OFFSET           16   /* offset of Top Vertical Border or Bottom Vertical Border in Lines */
#define HDMI_EDID_FLAGS_OFFSET              17   /* offset of flags */

#define HDMI_IEEE_REGISTRATION_ID        0x000c03  /* IEEE ID */

/* HDMI Connected States */
#define HDMI_STATE_NOMONITOR    0           /* No HDMI monitor connected*/
#define HDMI_STATE_CONNECTED    1           /* HDMI monitor connected but powered off */
#define HDMI_STATE_ON           2           /* HDMI monitor connected and powered on*/

/* HDMI EDID Length */
#define HDMI_EDID_MAX_LENGTH            512 /* max length of edid data */
#define HDMI_EDID_BLOCK_LENGTH          128 /* echo block length of edid data */

/* HDMI EDID DTDs */
#define HDMI_EDID_MAX_DTDS              4   /* max number of DTDs block */

/* HDMI EDID DTD Tags */
#define HDMI_EDID_DTD_TAG_MONITOR_NAME          0xFC  /* tag of monitor name of dtd block */
#define HDMI_EDID_DTD_TAG_MONITOR_SERIALNUM     0xFF  /* tag of monitor serialnum of dtd block */
#define HDMI_EDID_DTD_TAG_MONITOR_LIMITS        0xFD  /* tag of monitor limits of dtd block */
#define HDMI_EDID_DTD_TAG_STANDARD_TIMING_DATA  0xFA  /* tag of standard timing data of dtd block */
#define HDMI_EDID_DTD_TAG_COLOR_POINT_DATA      0xFB  /* tag of color point data of dtd block */
#define HDMI_EDID_DTD_TAG_ASCII_STRING          0xFE  /* tag of ascii string of dtd block */

#define HDMI_IMG_FORMAT_MAX_LENGTH              40    /* max length of video block */
#define HDMI_VIDEO_NATIVE_DTDS_MASK             0x0f  /* mask of video native DTDs */
#define HDMI_AUDIO_FORMAT_MAX_LENGTH            10    /* max length of audio block */
#define HDMI_AUDIO_BASIC_MASK                   0x40  /* mask of audio basic */

/* HDMI EDID Extenion Data Block Values: Video */
#define HDMI_EDID_EX_VIDEO_NATIVE               0x80  /* mask of native in video block */
#define HDMI_EDID_EX_VIDEO_MASK                 0x7F  /* mask of code in video block */

#define HDMI_CODE_VESA_OFFSET   100     /* offset of VESA code with CEA code */

/* macros of CEA timing code */
#define HDMI_EDID_EX_VIDEO_640x480p_60Hz_4_3        1
#define HDMI_EDID_EX_VIDEO_720x480p_60Hz_4_3        2
#define HDMI_EDID_EX_VIDEO_720x480p_60Hz_16_9       3
#define HDMI_EDID_EX_VIDEO_1280x720p_60Hz_16_9      4
#define HDMI_EDID_EX_VIDEO_1920x1080i_60Hz_16_9     5
#define HDMI_EDID_EX_VIDEO_720x480i_60Hz_4_3        6
#define HDMI_EDID_EX_VIDEO_720x480i_60Hz_16_9       7
#define HDMI_EDID_EX_VIDEO_720x240p_60Hz_4_3        8
#define HDMI_EDID_EX_VIDEO_720x240p_60Hz_16_9       9
#define HDMI_EDID_EX_VIDEO_2880x480i_60Hz_4_3       10
#define HDMI_EDID_EX_VIDEO_2880x480i_60Hz_16_9      11
#define HDMI_EDID_EX_VIDEO_2880x240p_60Hz_4_3       12
#define HDMI_EDID_EX_VIDEO_2880x240p_60Hz_16_9      13
#define HDMI_EDID_EX_VIDEO_1440x480p_60Hz_4_3       14
#define HDMI_EDID_EX_VIDEO_1440x480p_60Hz_16_9      15
#define HDMI_EDID_EX_VIDEO_1920x1080p_60Hz_16_9     16
#define HDMI_EDID_EX_VIDEO_720x576p_50Hz_4_3        17
#define HDMI_EDID_EX_VIDEO_720x576p_50Hz_16_9       18
#define HDMI_EDID_EX_VIDEO_1280x720p_50Hz_16_9      19
#define HDMI_EDID_EX_VIDEO_1920x1080i_50Hz_16_9     20
#define HDMI_EDID_EX_VIDEO_720x576i_50Hz_4_3        21
#define HDMI_EDID_EX_VIDEO_720x576i_50Hz_16_9       22
#define HDMI_EDID_EX_VIDEO_720x288p_50Hz_4_3        23
#define HDMI_EDID_EX_VIDEO_720x288p_50Hz_16_9       24
#define HDMI_EDID_EX_VIDEO_2880x576i_50Hz_4_3       25
#define HDMI_EDID_EX_VIDEO_2880x576i_50Hz_16_9      26
#define HDMI_EDID_EX_VIDEO_2880x288p_50Hz_4_3       27
#define HDMI_EDID_EX_VIDEO_2880x288p_50Hz_16_9      28
#define HDMI_EDID_EX_VIDEO_1440x576p_50Hz_4_3       29
#define HDMI_EDID_EX_VIDEO_1440x576p_50Hz_16_9      30
#define HDMI_EDID_EX_VIDEO_1920x1080p_50Hz_16_9     31
#define HDMI_EDID_EX_VIDEO_1920x1080p_24Hz_16_9     32
#define HDMI_EDID_EX_VIDEO_1920x1080p_25Hz_16_9     33
#define HDMI_EDID_EX_VIDEO_1920x1080p_30Hz_16_9     34
#define HDMI_EDID_EX_VIDEO_2880x480p_60Hz_4_3       35
#define HDMI_EDID_EX_VIDEO_2880x480p_60Hz_16_9      36
#define HDMI_EDID_EX_VIDEO_2880x576p_60Hz_4_3       37
#define HDMI_EDID_EX_VIDEO_2880x576p_60Hz_16_9      38
#define HDMI_EDID_EX_VIDEO_1920x1250i_50Hz_16_9     39
#define HDMI_EDID_EX_VIDEO_1920x1080i_100Hz_16_9    40
#define HDMI_EDID_EX_VIDEO_1280x720p_100Hz_16_9     41
#define HDMI_EDID_EX_VIDEO_720x576p_100Hz_4_3       42
#define HDMI_EDID_EX_VIDEO_720x576p_100Hz_16_9      43
#define HDMI_EDID_EX_VIDEO_720x576i_100Hz_4_3       44
#define HDMI_EDID_EX_VIDEO_720x576i_100Hz_16_9      45
#define HDMI_EDID_EX_VIDEO_1920x1080i_120Hz_16_9    46
#define HDMI_EDID_EX_VIDEO_1280x720p_120Hz_16_9     47
#define HDMI_EDID_EX_VIDEO_720x480p_120Hz_4_3       48
#define HDMI_EDID_EX_VIDEO_720x480p_120Hz_16_9      49
#define HDMI_EDID_EX_VIDEO_720x480i_120Hz_4_3       50
#define HDMI_EDID_EX_VIDEO_720x480i_120Hz_16_9      51
#define HDMI_EDID_EX_VIDEO_720x576p_200Hz_4_3       52
#define HDMI_EDID_EX_VIDEO_720x576p_200Hz_16_9      53
#define HDMI_EDID_EX_VIDEO_720x576i_200Hz_4_3       54
#define HDMI_EDID_EX_VIDEO_720x576i_200Hz_16_9      55
#define HDMI_EDID_EX_VIDEO_720x480p_240Hz_4_3       56
#define HDMI_EDID_EX_VIDEO_720x480p_240Hz_16_9      57
#define HDMI_EDID_EX_VIDEO_720x480i_240Hz_4_3       58
#define HDMI_EDID_EX_VIDEO_720x480i_240Hz_16_9      59

/* macros of VESA timing code */
#define HDMI_EDID_VESA_VIDEO_640x350p_85Hz_4_3          1+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_640x400p_85Hz_4_3          2+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_720x400p_85Hz_4_3          3+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_640x480p_60Hz_4_3          4+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_640x480p_72Hz_4_3          5+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_640x480p_75Hz_4_3          6+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_640x480p_85Hz_4_3          7+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_800x600p_56Hz_4_3          8+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_800x600p_60Hz_4_3          9+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_800x600p_72Hz_4_3          10+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_800x600p_75Hz_4_3          11+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_800x600p_85Hz_4_3          12+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_848x480p_60Hz_4_3          14+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_1024x768p_43Hz_4_3         15+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_1024x768p_60Hz_4_3         16+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_1024x768p_70Hz_4_3         17+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_1024x768p_75Hz_4_3         18+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_1024x768p_85Hz_4_3         19+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_1152x864p_75Hz_4_3         21+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_1280x768p_60Hz_CVT_16_9    22+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_1280x768p_60Hz_16_9        23+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_1280x768p_75Hz_16_9        24+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_1280x768p_85Hz_16_9        25+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_1280x800p_60Hz_CVT_16_9    27+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_1280x800p_60Hz_16_9        28+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_1280x800p_75Hz_16_9        29+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_1280x800p_85Hz_16_9        30+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_1280x960p_60Hz_4_3         32+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_1280x960p_85Hz_4_3         33+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_1280x1024p_60Hz_4_3        35+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_1280x1024p_75Hz_4_3        36+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_1280x1024p_85Hz_4_3        37+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_1360x768p_60Hz_16_9        39+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_1400x1050p_60Hz_CVT_4_3    41+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_1400x1050p_60Hz_4_3        42+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_1400x1050p_75Hz_4_3        43+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_1400x1050p_85Hz_4_3        44+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_1440x900p_60Hz_CVT_16_9    46+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_1440x900p_60Hz_16_9        47+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_1440x900p_75Hz_16_9        48+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_1440x900p_85Hz_16_9        49+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_1600x1200p_60Hz_4_3        51+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_1600x1200p_65Hz_4_3        52+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_1600x1200p_70Hz_4_3        53+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_1600x1200p_75Hz_4_3        54+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_1600x1200p_80Hz_4_3        55+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_1680x1050p_60Hz_CVT_16_9   57+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_1680x1050p_60Hz_16_9       58+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_1680x1050p_75Hz_16_9       59+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_1680x1050p_85Hz_16_9       60+HDMI_CODE_VESA_OFFSET
#define HDMI_EDID_VESA_VIDEO_1920x1080p_60Hz_16_9       82+HDMI_CODE_VESA_OFFSET

#define HDMI_TIMINGS_VESA_START                     27  /* start of VESA timing code in timing array */
#define HDMI_TIMINGS_VESA_END                       95  /* end of VESA timing code in timing array */

#define HDMI_CODE_TYPE_CEA      1   /* timing mode which timing code followed CEA */
#define HDMI_CODE_TYPE_VESA     0   /* timing mode which timing code followed VESA */

/* default timing mode */
#define HDMI_DEFAULT_TIMING_MODE  HDMI_CODE_TYPE_CEA
/* default timing code */
#define HDMI_DEFAULT_TIMING_CODE  HDMI_EDID_EX_VIDEO_1920x1080p_30Hz_16_9//HDMI_EDID_EX_VIDEO_1280x720p_60Hz_16_9
#define HDMI_DEFAULT_MHL_TIMING_CODE  HDMI_EDID_EX_VIDEO_1920x1080p_30Hz_16_9//HDMI_EDID_EX_VIDEO_1280x720p_60Hz_16_9

#define HDMI_DEFAULT_VESA_TIMING_CODE  HDMI_EDID_VESA_VIDEO_1024x768p_60Hz_4_3
#ifndef HDMI_MAX_VESA_TIMING_CODE
#define HDMI_MAX_VESA_TIMING_CODE      HDMI_EDID_VESA_VIDEO_1440x900p_75Hz_16_9
#endif

/* invalid timing code */
#define INVALID_VALUE   -1
/* timing order of mhl not support, refer to timing_order[]*/
#define MHL_NOT_SUPPORT_TIMING_ORDER  10   //12 change to 10, if supporting PP mode, need to change to 12

#define MHL_PPMODE_PIXEL_CLOCK     74250

/* audio type */
#define HDMI_EDID_EX_AUDIO_PCM      1
#define HDMI_EDID_EX_AUDIO_AC3      2
#define HDMI_EDID_EX_AUDIO_MP1      3
#define HDMI_EDID_EX_AUDIO_MP3      4
#define HDMI_EDID_EX_AUDIO_MP2      5
#define HDMI_EDID_EX_AUDIO_AAC      6
#define HDMI_EDID_EX_AUDIO_DTS      7
#define HDMI_EDID_EX_AUDIO_ATRAC    8
#define HDMI_EDID_EX_AUDIO_DSD      9
#define HDMI_EDID_EX_AUDIO_EAC3     10
#define HDMI_EDID_EX_AUDIO_DTS_HD   11
#define HDMI_EDID_EX_AUDIO_MLP      12
#define HDMI_EDID_EX_AUDIO_DST      13
#define HDMI_EDID_EX_AUDIO_WMA      14

/* audio channel number */
#define HDMI_EDID_EX_AUDIO_CHN_NUM_2      1
#define HDMI_EDID_EX_AUDIO_CHN_NUM_3      2
#define HDMI_EDID_EX_AUDIO_CHN_NUM_4      3
#define HDMI_EDID_EX_AUDIO_CHN_NUM_5      4
#define HDMI_EDID_EX_AUDIO_CHN_NUM_6      5
#define HDMI_EDID_EX_AUDIO_CHN_NUM_7      6
#define HDMI_EDID_EX_AUDIO_CHN_NUM_8      7

/* audio freq */
#define HDMI_EDID_EX_AUDIO_FREQUENCY_32K     (1<<0)
#define HDMI_EDID_EX_AUDIO_FREQUENCY_44_1K   (1<<1)
#define HDMI_EDID_EX_AUDIO_FREQUENCY_48K     (1<<2)
#define HDMI_EDID_EX_AUDIO_FREQUENCY_88_2K   (1<<3)
#define HDMI_EDID_EX_AUDIO_FREQUENCY_96K     (1<<4)
#define HDMI_EDID_EX_AUDIO_FREQUENCY_176_4K  (1<<5)
#define HDMI_EDID_EX_AUDIO_FREQUENCY_192K    (1<<6)

/* default audio param */
#if HDMI_DEFAULT_PCM_PLAY
#define HDMI_DEFAULT_AUDIO_TYPE        HDMI_EDID_EX_AUDIO_PCM
#define HDMI_DEFAULT_AUDIO_CHANNL_NUM  HDMI_EDID_EX_AUDIO_CHN_NUM_2
#else
#define HDMI_DEFAULT_AUDIO_TYPE        HDMI_EDID_EX_AUDIO_EAC3
#define HDMI_DEFAULT_AUDIO_CHANNL_NUM  HDMI_EDID_EX_AUDIO_CHN_NUM_8
#endif
#define HDMI_DEFAULT_AUDIO_FREQUENCY   0x7F //support all freq type

/*  Video Descriptor Block  */
typedef struct _hdmi_edid_dtd_video {
    u16 pixel_clock;          /* 54-55 */
    u8  horiz_active;         /* 56 */
    u8  horiz_blanking;       /* 57 */
    u8  horiz_high;           /* 58 */
    u8  vert_active;          /* 59 */
    u8  vert_blanking;        /* 60 */
    u8  vert_high;            /* 61 */
    u8  horiz_sync_offset;    /* 62 */
    u8  horiz_sync_pulse;     /* 63 */
    u8  vert_sync_pulse;      /* 64 */
    u8  sync_pulse_high;      /* 65 */
    u8  horiz_image_size;     /* 66 */
    u8  vert_image_size;      /* 67 */
    u8  image_size_high;      /* 68 */
    u8  horiz_border;         /* 69 */
    u8  vert_border;          /* 70 */
    u8  misc_settings;        /* 71 */
} hdmi_edid_dtd_video;

/* Monitor Limits Descriptor Block */
typedef struct _hdmi_edid_dtd_monitor {
    u16 pixel_clock;          /* 54-55*/
    u8  _reserved1;           /* 56 */
    u8  block_type;           /* 57 */
    u8  _reserved2;           /* 58 */
    u8  min_vert_freq;        /* 59 */
    u8  max_vert_freq;        /* 60 */
    u8  min_horiz_freq;       /* 61 */
    u8  max_horiz_freq;       /* 62 */
    u8  pixel_clock_mhz;      /* 63 */
    u8  GTF[2];               /* 64 -65 */
    u8  start_horiz_freq;     /* 66 */
    u8  C;                    /* 67 */
    u8  M[2];                 /* 68-69 */
    u8  K;                    /* 70 */
    u8  J;                    /* 71 */

} __attribute__ ((packed)) hdmi_edid_dtd_monitor;

/* Text Descriptor Block */
typedef struct _hdmi_edid_dtd_text {
    u16 pixel_clock;          /* 54-55 */
    u8  _reserved1;           /* 56 */
    u8  block_type;           /* 57 */
    u8  _reserved2;           /* 58 */
    u8  text[13];             /* 59-71 */
} __attribute__ ((packed)) hdmi_edid_dtd_text;

/* DTD Union */
typedef union _hdmi_edid_dtd {
    hdmi_edid_dtd_video     video;                  /* video timing data block of dtd block */
    hdmi_edid_dtd_text      monitor_name;           /* monitor name block of dtd block */
    hdmi_edid_dtd_text      monitor_serial_number;  /* monitor serial number block of dtd block */
    hdmi_edid_dtd_text      ascii;                  /* ascii string block of dtd block */
    hdmi_edid_dtd_monitor   monitor_limits;         /* monitor limits block of dtd block */
} __attribute__ ((packed)) hdmi_edid_dtd;

/*    EDID struct    */
typedef struct _hdmi_edid {
    u8  header[8];            /* 00-07  edid的头，8个字节，为固定值(00 FF FF FF FF FF FF 00)，
                                在1.4中为必须设置的*/
    u16 manufacturerID;       /* 08-09  基于ASCII码，最高位设置为0,1没有使用。
                                后面分别的三个五位对应五位编码的ASCII*/
    u16 product_id;           /* 10-11 2字节，用于区别同一个生产商的不同产品，例如模块名*/
    u32 serial_number;        /* 12-15 用于区别相同显示模式下不同个体，应该是不同单个产品，该使用是可选的
                                序列号是唯一的和ASCII不对应，范围为0到4,294,967,295，如果没有使用，则为全0*/

    u8  week_manufactured;    /* 16 该字节为可选，范围为1-54，如果没有使用该字节，则设置为00h*/
    u8  year_manufactured;    /* 17 代表产品生产的年份或是模块年份，如果用于代表模块年份，
                                则week字节要设置为FFh,而该字节存储的是从以下的函数计算得出的数据
                                (Year of Manufacture {or Model Year} - 1990)*/

    u8  edid_version;         /* 18 版本号 当前为1*/
    u8  edid_revision;        /* 19 版本修订号 当前为4*/

    /*基本显示参数和特性---5字节*/
    u8  video_in_definition;  /* 20 源设备需要根据当前字节来确认自身设备的输出视频特性，
                                具体格式，详见3.11ofEDIDrAr2,最高位分别表示数字信号和模拟信号
                                模拟信号:6、5位信号水平标准,4位video设置，3、2、1位同步类型，0位锯齿
                                数字信号:6\5\4位色深，3、2、1、0数字视频标准支持接口
                                如果第7位设置为1，那么24字节的4和3位定义为支持的颜色编码格式
                                如果显示支持多于一种以上的同步种类，同步H&V分隔信号会拥有更高
                                的权限，水平复合同步信号有比较低的权限*/

    /*源设备通过使用这个数据来获取一个大概的图像尺寸来产生一个缩放到合适大小的文字和图像
       21,22字节表示水平和垂直屏幕尺寸，两字节如果有一个设置为0，则21字节表示横屏宽高比，22表示竖屏宽高比*/
    u8  max_horiz_image_size; /* 21 表示设备显示尺寸或是宽高比；表示显示尺寸时，该字节表示水平
                                宽度，精确到cm。当表示宽高比时，则表示为横屏宽高比
                                Aspect Ratio = (Stored Value + 99) ÷ 100*/
    u8  max_vert_image_size;  /* 22 同上，表示垂直高度，精确到cm。当表示宽高比时，则表示竖屏宽高比*/



    u8  display_gamma;        /* 23 显示传输特征，范围1.00 到 3.54 存储值遵循以下公式
                                Stored Value = (GAMMA x 100) – 100
                                如果设置为ffh，那么该字节没有定义，而是在扩展块中进行的定义
                                GAMMA值为生产厂商定义*/


    u8  power_features;       /* 24 用来显示支持的不同显示特征，详见3.14ofEDIDrAr2
                                7、6、5位为显示电源管理，如果第7位设置为0，
                                那么第4、3位为显示颜色类型，如果设置为1,那么第4、3位为颜色编码格式
                                2、1、0三位表示其他支持特征*/


    /**/
    u8  chroma_info[10];      /* 25-34 高9位到2位作为一个字节，低两位1-0和前一个字节的地两位作为一对。
                                所有值应该精确到+/-0.0005 单色显示不考虑实际颜色，
                                只用来显示相应的白点的xy并设置相应的红绿蓝xy为00h
                                详见table3.17*/

    /*Established Timings I & II:3个字节,是一个1bit标志列表，
    用来显示对VESA和其他压缩格式的通用Timing，显示的是Factory Supported Modes,工厂模式
    不用来表示显示设备的局限范围，而是一旦设置为1，则表示Factory Supported Modes
    任何一个Timing都可以用Detailed Timings data field defined来描述*/
    u8  timing_1;             /* 35 */
    u8  timing_2;             /* 36 */
    u8  timing_3;             /* 37 6-0位可能用来标识生产者私有Timing,VESA对此没有相应定义*/

    /*Standard Timings 16字节
    没有用到的区域会被设置为01h，
    所有Standard Timings会被定义为等比像素(1:1)详见table3.19*/
    u8  std_timings[16];      /* 38-53 */

    /*72个字节分为4个数据区，每个18个字节长度，这些区域要么是detailed timing data
    要么是其他数据类型36h → 47h第一 Preferred Timing Mode
    48h → 59h 第二 2nd Detailed Timing Descriptor or the 1st Display Descriptor
    5Ah → 6Bh 第三
    6Ch → 7Dh 第四*/
    hdmi_edid_dtd dtd[4];     /* 54-125
                                1、Detailed Timing Descriptor可能定义4个数据域，分两部分，Detailed Timing Definition
                                2、Display Descriptor Definitions可选，前一个块为Preferred Timing Mode
                                   后三个可能包含video timing definitions或者使用通用格式DisplayDescriptors
                                   如果前三个字节都为0，那么包含的是DisplayDescriptors，第四个字节要包含tag数字，第五个字节为00h
                                   tag为FFh，Display Product Serial Number Descriptor Definition
                                   tag为FEh，Alphanumeric Data String Descriptor
                                   tag为FDh，Display Range Limits & Additional Timing Descriptor Definition，可选
                                   tag为FCh，Display Product Name (ASCII) String Descriptor Definition
                                   FBh       Color Point Descriptor Definition
                                   FAh       Standard Timing Identifier Definition
                                   F9h       Color Management Data Definition
                                   F8h       CVT 3 Byte Code Descriptor Definition
                                   F7h       Established Timings III Descriptor Definition
                                   11h to F6h  Unused – Reserved Data Tag Number
                                   10h       Dummy Descriptor Definition
                                   00h to 0Fh  Manufacturer Specified Data Tag Numbers
                              */

    u8  extension_edid;       /* 126 EDID扩展块的数量
                                源设备会执行checksum校验*/
    u8  checksum;             /* 127 */
    u8  extension_tag;        /* 00 (extensions follow EDID) */
    u8  extention_rev;        /* 01 */
    u8  offset_dtd;           /* 02 */
    u8  num_dtd;              /* 03 */

    u8  data_block[123];      /* 04 - 126 */
    u8  extension_checksum;   /* 127 */

    u8  ext_datablock[256];   /* extend data block */
} __attribute__ ((packed)) hdmi_edid;

typedef struct _hdmi_video_timings {

    u16 x_res;          /* Unit: pixels */
    u16 y_res;          /* Unit: pixels */

    u32 pixel_clock;    /* Unit: KHz */

    u16 hsw;    /* Horizontal synchronization pulse width, Unit: pixel clocks */
    u16 hfp;    /* Horizontal front porch, Unit: pixel clocks */
    u16 hbp;    /* Horizontal back porch, Unit: pixel clocks */

    u16 vsw;    /* Vertical synchronization pulse width, Unit: line clocks */
    u16 vfp;    /* Vertical front porch, Unit: line clocks */
    u16 vbp;    /* Vertical back porch, Unit: line clocks */

    u8 pix_rep;   /*pixel repetition*/
    int aspect_ratio;   /* aspect ratio*/
    u16 freq;  /*Vertical Frequency*/
} hdmi_video_timings;

typedef struct _hdmi_hvsync_pol {
    int vsync_pol;              /* horizontal polarity */
    int hsync_pol;              /* vertical polarity */
} hdmi_hvsync_pol;

typedef enum _extension_edid_db {
    DATABLOCK_AUDIO    = 1,     /* index of audio block in cea */
    DATABLOCK_VIDEO    = 2,     /* index of video block in cea */
    DATABLOCK_VENDOR   = 3,     /* index of vendor block in cea */
    DATABLOCK_SPEAKERS = 4,     /* index of speakers block in cea */
    DATABLOCK_EXTENDED_TAG = 7, /* index of extended tag block in cea */
} extension_edid_db;

typedef struct _img_edid {
    bool pref;                  /* video timing native */
    int code;                   /* video timing code */
} img_edid;

typedef struct _image_format {
    int number;                                 /* number of video timing */
    img_edid fmt[HDMI_IMG_FORMAT_MAX_LENGTH];   /* array of video timing code */
} image_format;

typedef struct _audio_edid {
    int num_of_ch;              /* number of audio channel */
    int format;                 /* audio format */
    int sampling_freq;          /* audio frequency */
} audio_edid;

typedef struct _audio_format {
    int number;                                     /* number of audio timing */
    audio_edid fmt[HDMI_AUDIO_FORMAT_MAX_LENGTH];   /* array of audio type */
} audio_format;

typedef struct _latency {
    /* vid: if indicated, value=1+ms/2 with a max of 251 meaning 500ms */
    int vid_latency;            /* video latency */
    int aud_latency;            /* audio latency */
    int int_vid_latency;        /* interlaced video latency */
    int int_aud_latency;        /* interlaced audio latency */
} latency;

typedef struct _deep_color {
    bool bit_30;                /* deep color 30 bit support */
    bool bit_36;                /* deep color 36 bit support */
    int max_tmds_freq;          /* max tmds clock frequency */
} deep_color;

typedef struct _hdmi_cm {
    int code;                   /* video timing code */
    int mode;                   /* video mode: 0 for VESA, 1 for CEA  */
} hdmi_cm;

typedef enum _edid_timing_mode {
    EDID_TIMING_MODE_P,         /* video timing pronounced mode */
    EDID_TIMING_MODE_I          /* video timing interleaved mode */
} edid_timing_mode;

//read edid
int  edid_read(u8* edid);

//timings
int  edid_get_image_format(u8 *edid, image_format *format);
int  edid_get_audio_format(u8 *edid, audio_format *format);
int  edid_get_timings_index(int mode, int code);
int  edid_get_s3d_timings_index(int mode, int code);
hdmi_video_timings* edid_get_timings_byindex(int index);
hdmi_hvsync_pol* edid_get_hvpol_byindex(int index);
edid_timing_mode edid_get_timing_mode(int code);
hdmi_cm edid_get_timing_code(hdmi_edid *edid, bool s3d_enabled);
hdmi_cm edid_get_best_timing(hdmi_edid *edid);

void edid_get_deep_color_info(u8 *edid, deep_color *format);

void edid_dump_video_timings(hdmi_video_timings *timings);

bool edid_has_ieee_id(u8 *edid);
bool edid_ai_supported(u8 *edid);
bool edid_s3d_supported(u8 *edid);
bool edid_is_valid_code(int mode, int code);
int  edid_get_timing_order(int code, bool mhl_check);
int  edid_get_vesa_timing_order(int code, bool mhl_check);
int  edid_get_default_code(void);
bool edid_is_valid_edid(u8 *edid);
bool edid_has_vcdb(u8 *edid);
bool edid_cec_getphyaddr(u8 *edid, u8* physical_addr);

//#if USE_PP_MODE
bool edid_supported_yuv422(u8 *edid);
//#endif

#endif

