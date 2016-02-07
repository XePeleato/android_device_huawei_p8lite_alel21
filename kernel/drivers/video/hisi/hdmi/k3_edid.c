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

#include "k3_fb.h"

#include "k3_edid.h"
#include "k3_hdmi.h"
//#include "k3_hdmi_hw.h"
#include "k3_hdmi_hw_tpi.h"

#define LOG_TAG "hdmi-edid"
#include "k3_hdmi_log.h"

/* edid */
static u8 edid_header[8] = {0x0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0};

/* All supported timing values that  supports */
static hdmi_video_timings all_timings_direct[] = {
    { 640,  480,  25200,  96,  16,  48, 2, 10, 33, 0, 1, 6000}, /* 1     -- 0 */
    {1280,  720,  74250,  40, 440, 220, 5,  5, 20, 0, 2, 5000}, /* 19 */
    {1280,  720,  74250,  40, 110, 220, 5,  5, 20, 0, 2, 6000}, /* 4 */
    { 720,  480,  27000,  62,  16,  60, 6,  9, 30, 0, 1, 6000}, /* 2,3 */
    {2880,  576, 108000, 256,  48, 272, 5,  5, 39, 0, 1, 5000}, /* 37 */
    {1440,  240,  27000, 124,  38, 114, 3,  4, 15, 1, 1, 6000}, /* 6,7   -- 5 */
    {1440,  288,  27000, 126,  24, 138, 3,  2, 19, 1, 1, 5000}, /* 21,22 */
    {1920,  540,  74250,  44, 528, 148, 5,  2, 15, 0, 2, 5000}, /* 20 */
    {1920,  540,  74250,  44,  88, 148, 5,  2, 15, 0, 2, 6000}, /* 5 */
    {1920, 1080, 148500,  44,  88, 148, 5,  4, 36, 0, 2, 6000}, /* 16 */
    { 720,  576,  27000,  64,  12,  68, 5,  5, 39, 0, 1, 5000}, /* 17,18 -- 10 */
    {1440,  576,  54000, 128,  24, 136, 5,  5, 39, 0, 1, 5000}, /* 29,30 */
    {1920, 1080, 148500,  44, 528, 148, 5,  4, 36, 0, 1, 5000}, /* 31 */
    {2880,  480, 108000, 248,  64, 240, 6,  9, 30, 0, 1, 5994}, /* 35,36 */
    {1920, 1080,  74250,  44, 638, 148, 5,  4, 36, 0, 2, 2400}, /* 32 */
    {1920, 1080,  74250,  44, 528, 148, 5,  4, 36, 0, 2, 2500}, /* 33 */
    {1920, 1080,  74250,  44,  88, 148, 5,  4, 36, 0, 2, 3000}, /* 34 */
    { 720,  480,  27030,  62,  16,  60, 6,  9, 30, 0, 1, 6000}, /* 3     -- 40 *///to do dont find
    {1440,  240,  27000, 124,  38, 114, 3,  4, 15, 1, 1, 6000}, /* 8,9 */
    {1440,  480,  54000, 124,  32, 120, 6,  9, 30, 0, 1, 6000}, /* 14,15 */
    {1440,  288,  27000, 126,  24, 138, 3,  2, 19, 1, 1, 5000}, /* 23,24 */
    {1280,  720, 148500,  40, 400, 220, 5,  5, 20, 0, 2, 10000}, /* 41 */
    { 720,  576,  54000,  64,  12,  68, 5,  5, 39, 0, 1, 10000}, /* 42,43 -- 45 */
    {1280,  720, 148350,  40, 110, 220, 5,  5, 20, 0, 2, 11988}, /* 47 */
    { 720,  480,  54000,  62,  16,  60, 6,  1, 30, 0, 1, 11988}, /* 48,49 */
    { 720,  576, 108000,  64,  12,  68, 5,  5, 39, 0, 1, 20000}, /* 52,53 */
    { 720,  480, 108000,  62,  16,  60, 6,  9, 30, 0, 1, 24000}  /* 56,57 */
    /* Vesa from here */
    ,
    { 640,  350,  31500,  64,  32,  96, 3, 32, 60, 0, 2, 8508}, /*  1, 640 x 350 @ 85Hz; */
    { 640,  400,  31500,  64,  32,  96, 3,  1, 41, 0, 2, 8508}, /*  2, 640 x 400 @ 85Hz; */
    { 720,  400,  35500,  72,  36, 108, 3,  1, 42, 0, 2, 8504}, /*  3, 720 x 400 @ 85Hz; */
    { 640,  480,  25175,  96,  16,  48, 2, 10, 33, 0, 1, 5994}, /*  4, 640 x 480 @ 60Hz; -- 30 */
    { 640,  480,  31500,  40,  128, 128,3,  9, 28, 0, 1, 7281}, /*  5, 640 x 480 @ 72Hz; */
    { 640,  480,  31500,  64,  24, 128, 3,  9, 28, 0, 1, 7500}, /*  6, 640 x 480 @ 75Hz; */
    { 640,  480,  36000,  56,  24, 128, 3,  9, 25, 0, 1, 8500}, /*  7, 640 x 480 @ 85Hz; */
    { 800,  600,  36000,  72,  24, 128, 2,  1, 22, 0, 1, 5625}, /*  8, 800 x 600 @ 56Hz; */
    { 800,  600,  40000, 128,  40,  88, 4,  1, 23, 0, 1, 6032}, /*  9, 800 x 600 @ 60Hz; -- 35 */
    { 800,  600,  50000, 120,  56,  64, 6, 37, 23, 0, 1, 7219}, /* 10, 800 x 600 @ 72Hz; */
    { 800,  600,  49500,  80,  16, 160, 3,  1, 21, 0, 1, 7500}, /* 11, 800 x 600 @ 75Hz; */
    { 800,  600,  56250,  64,  32, 152, 3,  1, 27, 0, 1, 8506}, /* 12, 800 x 600 @ 85Hz; */
    { 800,  600,  73250,  32,  48,  80, 3,  4, 29, 0, 0, 0}, /* 13, 800 x 600 @ 120Hz;*/// to do dont find
    { 848,  480,  33750, 112,  16, 112, 8,  6, 23, 0, 0, 0}, /* 14, 848 x 480 @ 60Hz; -- 40 */// to do dont find
    {1024,  768,  44900, 176,   8,  56, 4,  0, 20, 0, 0, 0}, /* 15, 1024 x 768 @ 43Hz */// to do dont find
    {1024,  768,  65000, 136,  24, 160, 6,  3, 29, 0, 1, 6000}, /* 16, 1024 x 768 @ 60Hz */
    {1024,  768,  75000, 136,  24, 144, 6,  3, 19, 0, 1, 7007}, /* 17, 1024 x 768 @ 70Hz */
    {1024,  768,  78750,  96,  16, 176, 3,  1, 28, 0, 1, 7503}, /* 18, 1024 x 768 @ 75Hz */
    {1024,  768,  94500,  96,  48, 208, 3,  1, 36, 0, 1, 8500}, /* 19, 1024 x 768 @ 85Hz -- 45 */
    {1024,  768, 115500,  32,  48,  80, 4,  3, 38, 0, 0, 0}, /* 20, 1024 x 768 @ 120Hz */// to do dont find
    {1152,  864, 108000, 128,  64, 256, 3,  1, 32, 0, 1, 7500}, /* 21, 1152 x 864 @ 75Hz; */
    {1280,  768,  68250,  32,  48,  80, 7,  3, 12, 0, 2, 6000}, /* 22, 1280 x 768 @ 60Hz CVT */
    {1280,  768,  79500, 128,  64, 192, 7,  3, 20, 0, 2, 5987}, /* 23, 1280 x 768 @ 60Hz */
    {1280,  768, 102250, 128,  80, 208, 7,  3, 27, 0, 2, 6029}, /* 24, 1280 x 768 @ 75Hz  -- 50 */
    {1280,  768, 117500, 136,  80, 216, 7,  3, 31, 0, 2, 8484}, /* 25, 1280 x 768 @ 85Hz */
    {1280,  768, 140250,  32,  48,  80, 7,  3, 35, 0, 0, 0}, /* 26, 1280 x 768 @ 120Hz */// to do dont find
    {1280,  800,  71000,  32,  48,  80, 6,  3, 14, 0, 0, 0}, /* 27, 1280 x 800 @ 60Hz CVT */// to do dont find
    {1280,  800,  83500, 128,  72, 200, 6,  3, 22, 0, 0, 0}, /* 28, 1280 x 800 @ 60Hz */// to do dont find
    {1280,  800, 106500, 128,  80, 208, 6,  3, 29, 0, 0, 0}, /* 29, 1280 x 800 @ 75Hz  -- 55 */// to do dont find
    {1280,  800, 122500, 136,  80, 216, 6,  3, 34, 0, 0, 0}, /* 30, 1280 x 800 @ 85Hz */// to do dont find
    {1280,  800, 146250,  32,  48,  80, 6,  3, 38, 0, 0, 0}, /* 31, 1280 x 800 @ 120Hz */// to do dont find
    {1280,  960, 108000, 112,  96, 312, 3,  1, 36, 0, 1, 6000}, /* 32, 1280 x 960 @ 60Hz */
    {1280,  960, 148500, 160,  64, 224, 3,  1, 47, 0, 1, 8500}, /* 33, 1280 x 960 @ 85Hz */
    {1280,  960, 175500,  32,  48,  80, 4,  3, 50, 0, 0, 0}, /* 34, 1280 x 960 @ 120Hz -- 60*/// to do dont find
    {1280, 1024, 108000, 112,  48, 248, 3,  1, 38, 0, 1, 6002}, /* 35, 1280 x 1024 @ 60Hz */
    {1280, 1024, 135000, 144,  16, 248, 3,  1, 38, 0, 1, 7502}, /* 36, 1280 x 1024 @ 75Hz */
    {1280, 1024, 157500, 160,  64, 224, 3,  1,  4, 0, 1, 8502}, /* 37, 1280 x 1024 @ 85Hz */
    {1280, 1024, 187250,  32,  48,  80, 7,  3, 44, 0, 0, 0}, /* 38, 1280 x 1024 @ 120Hz */// to do dont find
    {1360,  768,  85500, 112,  64, 256, 6,  3, 18, 0, 2, 6002}, /* 39, 1360 x 768 @ 60Hz  -- 65 */
    {1360,  768, 148250,  32,  48,  80, 5,  3, 37, 0, 0, 0}, /* 40, 1360 x 768 @ 120Hz */// to do dont find
    {1400, 1050, 101000,  32,  48,  80, 4,  3, 23, 0, 1, 5995}, /* 41, 1400 x 1050 @ 60Hz */
    {1400, 1050, 121750, 144,  88, 232, 4,  3, 32, 0, 1, 5998}, /* 42, 1400 x 1050 @ 60Hz */
    {1400, 1050, 156000, 144, 104, 248, 4,  3, 42, 0, 1, 7487}, /* 43, 1400 x 1050 @ 75Hz */
    {1400, 1050, 179500, 152, 104, 256, 4,  3, 48, 0, 1, 8496}, /* 44, 1400 x 1050 @ 85Hz -- 70 */
    {1400, 1050, 208000,  32,  48,  80, 4,  3, 55, 0, 0, 0}, /* 45, 1400 x 1050 @ 120Hz */ //to do dont find
    {1440,  900,  88750,  32,  48,  80, 6,  3, 17, 0, 0, 0}, /* 46, 1440 x 900 @ 60Hz CVT *///to do dont find
    {1440,  900, 106500, 152,  80, 232, 6,  3, 25, 0, 0, 0}, /* 47, 1440 x 900 @ 60Hz *///to do dont find
    {1440,  900, 136750, 152,  96, 248, 6,  3, 33, 0, 0, 0}, /* 48, 1440 x 900 @ 75Hz; *///to do dont find
    {1440,  900, 157000, 152, 104, 256, 6,  3, 39, 0, 0, 0}, /* 49, 1440 x 900 @ 85Hz; -- 75 *///to do dont find
    {1440,  900, 182750,  32,  48,  80, 6,  3, 44, 0, 0, 0}, /* 50, 1440 x 900 @ 120Hz; *///to do dont find
    {1600, 1200, 162000, 304,  64, 304, 3,  1, 46, 0, 0, 0}, /* 51, 1600 x 1200 @ 60Hz *///to do dont find
    {1600, 1200, 175500, 192,  64, 304, 3,  1, 46, 0, 1, 6500}, /* 52, 1600 x 1200 @ 65Hz */
    {1600, 1200, 189000, 192,  64, 304, 3,  1, 46, 0, 1, 7000}, /* 53, 1600 x 1200 @ 70Hz */
    {1600, 1200, 202500, 192,  64, 304, 3,  1, 46, 0, 1, 7500}, /* 54, 1600 x 1200 @ 75Hz -- 80 */
    {1600, 1200, 229500, 192,  64, 304, 3,  1, 46, 0, 1, 8500}, /* 55, 1600 x 1200 @ 85Hz */
    {1600, 1200, 268250,  32,  48,  80, 4,  3, 64, 0, 0, 0}, /* 56, 1600 x 1200 @ 120Hz *///to do dont find
    {1680, 1050, 119000,  32,  48,  80, 6,  3, 21, 0, 0, 0}, /* 57, 1680 x 1050 @ 60Hz CVT *///to do dont find
    {1680, 1050, 146250, 176, 104, 280, 6,  3, 30, 0, 0, 0}, /* 58, 1680 x 1050 @ 60Hz *///to do dont find
    {1680, 1050, 187000, 176, 120, 296, 6,  3, 40, 0, 0, 0}, /* 59, 1680 x 1050 @ 75Hz -- 85 *///to do dont find
    {1680, 1050, 214750, 176, 128, 304, 6,  3, 46, 0, 0, 0}, /* 60, 1680 x 1050 @ 85Hz *///to do dont find
    {1680, 1050, 245500,  32,  48,  80, 6,  3, 53, 0, 0, 0}, /* 61, 1680 x 1050 @ 120Hz *///to do dont find
    {1920, 1200, 154000,  32,  48,  80, 6,  3, 26, 0, 2, 5995}, /* 68, 1920 x 1200 @ 60Hz; CVT */
    {1920, 1200, 193250, 200, 136, 336, 6,  3, 36, 0, 2, 5988}, /* 69, 1920 x 1200 @ 60Hz; */
    {1920, 1200, 245250, 208, 136, 344, 6,  3, 46, 0, 2, 7493}, /* 70, 1920 x 1200 @ 75Hz -- 90 */
    {1920, 1200, 281250, 208, 144, 352, 6,  3, 53, 0, 2, 8493}, /* 71, 1920 x 1200 @ 85Hz; */
    {1920, 1200, 317000,  32,  48,  80, 6,  3, 62, 0, 0, 0}, /* 72, 1920 x 1200 @ 120Hz; *///to do dont find
    {1366,  768,  85500, 143,  70, 213, 3,  3, 24, 0, 0, 0}, /* 81 *///to do dont find
    {1920, 1080, 148500,  44,  88, 148, 5,  4, 36, 0, 2, 6000}, /* 82 == CEA 16*/
    {1280,  720,  74250,  40, 110, 220, 5,  5, 20, 0, 2, 6000}, /* 85 == CEA 4 -- 95 */
    /* supported 3d timings UNDEROVER full frame */
    {1280, 1470, 148350,  40, 110, 220, 5,  5, 20, 0, 0, 0},
    {1280, 1470, 148500,  40, 110, 220, 5,  5, 20, 0, 0, 0},
    {1280, 1470, 148500,  40, 440, 220, 5,  5, 20, 0, 0, 0}
};


/* Array which maps the timing values with corresponding CEA / VESA code */
static int code_index[ARRAY_SIZE(all_timings_direct)] = {
    /* <--0 CEA 26--> */
    /*      L2      L4      L6      L8   */
     1, 19,  4,  2, 37,  6, 21, 20,  5, 16,
    17, 29, 31, 35, 32, 33, 34,  3,  8, 14,
    23, 41, 42, 47, 48, 52, 56,
    /*   <--27 VESA 95-->   */   1,  2,  3,
     4,  5,  6,  7,  8,  9, 10, 11, 12, 13,
    14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
    24, 25, 26 ,27 ,28, 29, 30, 31, 32, 33,
    34, 35, 36, 37, 38, 39, 40, 41, 42, 43,
    44, 45, 46, 47, 48, 49, 50, 51, 52, 53,
    54, 55 ,56, 57, 58, 59, 60, 61, 68, 69,
    70, 71, 72, 81, 82, 85,
    /* <--96 3D 98--> */
     4,  4, 19
};

/* Mapping the Timing values with the corresponding Vsync and Hsync polarity */
static hdmi_hvsync_pol hvpol_mapping[ARRAY_SIZE(all_timings_direct)] = {
    /*L0              L2              L4              L6              L8         */
    {0, 0}, {1, 1}, {1, 1}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {1, 1}, {1, 1}, {1, 1},
    {0, 0}, {0, 0}, {1, 1}, {0, 0}, {1, 1}, {1, 1}, {1, 1}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {1, 1}, {0, 0}, {1, 1}, {0, 0}, {0, 0}, {0, 0},
    /*                       VESA                    */     {0, 0}, {1, 1}, {1, 1},
    {1, 0}, {1, 0}, {1, 1}, {1, 1}, {1, 1}, {0, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 1},
    {1, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {1, 1}, {1, 0},
                                                            {0, 0}, {1, 1}, {1, 1},
    {1, 0}, {1, 0}, {1, 1}, {1, 1}, {1, 1}, {0, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 1},
    {1, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {1, 1}, {1, 0}, {0, 0}, {1, 1}, {1, 1},
    {1, 0}, {1, 0}, {1, 1}, {1, 1}, {1, 1}, {0, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 1},
    {1, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {1, 1}, {1, 0}, {0, 0}, {1, 1}, {1, 1},
    {1, 0}, {1, 0}, {1, 1}, {1, 1}, {1, 1}, {0, 0},
    /*                   3D                   */    {1, 1}, {1, 1}, {1, 1}
};

/* Map CEA code to the corresponding timing values (10 entries/line) */
static int code_cea[60] = {
    /*      L2      L4      L6      L8   */
    -1,  0,  3,  3,  2, -1, -1, -1, 18, 18,
    -1, -1, -1, -1, 19, 19,  9, 10, 10,  1,
    -1, -1, -1, 20, 20, -1, -1, -1, -1, 11,
    11, 12, 14, 15, 16, -1, -1, -1, -1, -1,
    -1, 21, 22, 22, -1, -1, -1, 23, 24, 24,
    -1, -1, 25, 25, -1, -1, 26, 26, -1, -1
};

/* Map CEA code to the corresponding 3D timing values */
static int s3d_code_cea[60] = {
    /*       L2        L4        L6       L8   */
    -1, -1, -1, -1, 97, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, 98,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

/* Map VESA code to the corresponding timing values */
static int code_vesa[86] = {
    /*      L2      L4      L6      L8   */
    -1, 27, 28, 29, 30, 31, 32, 33, 34, 35,
    36, 37, 38, 39, 40, 41, 42, 43, 44, 45,
    46, 47, 48, 49, 50, 51, 52, 53, 54, 55,
    56, 57, 58, 59, 60, 61, 62, 63, 64, 65,
    66, 67, 68, 69, 70, 71, 72, 73, 74, 75,
    76, 77, 78, 79, 80, 81, 82, 83, 84, 85,
    86, 87, -1, -1, -1, -1, -1, -1, 88, 89,
    90, 91, 92, -1, -1, -1, -1, -1, -1, -1,
    -1, 93, 94, -1, -1, 95
};

static int timing_order[] = {
    HDMI_EDID_EX_VIDEO_640x480p_60Hz_4_3,
    HDMI_EDID_EX_VIDEO_720x480p_60Hz_4_3,
    HDMI_EDID_EX_VIDEO_720x480p_60Hz_16_9,
    HDMI_EDID_EX_VIDEO_720x576p_50Hz_4_3,
    HDMI_EDID_EX_VIDEO_720x576p_50Hz_16_9,
    HDMI_EDID_EX_VIDEO_1280x720p_50Hz_16_9,
    HDMI_EDID_EX_VIDEO_1280x720p_60Hz_16_9,
    HDMI_EDID_EX_VIDEO_1920x1080p_24Hz_16_9,
    HDMI_EDID_EX_VIDEO_1920x1080p_25Hz_16_9,
    HDMI_EDID_EX_VIDEO_1920x1080p_30Hz_16_9,
    HDMI_EDID_EX_VIDEO_1920x1080p_50Hz_16_9,
    HDMI_EDID_EX_VIDEO_1920x1080p_60Hz_16_9
};

static int vesa_timing_order[] = {
    HDMI_EDID_VESA_VIDEO_640x350p_85Hz_4_3,
    HDMI_EDID_VESA_VIDEO_640x400p_85Hz_4_3,
    HDMI_EDID_VESA_VIDEO_720x400p_85Hz_4_3,
    HDMI_EDID_VESA_VIDEO_640x480p_60Hz_4_3,
    HDMI_EDID_VESA_VIDEO_640x480p_72Hz_4_3,
    HDMI_EDID_VESA_VIDEO_640x480p_75Hz_4_3,
    HDMI_EDID_VESA_VIDEO_640x480p_85Hz_4_3,
    HDMI_EDID_VESA_VIDEO_800x600p_56Hz_4_3,
    HDMI_EDID_VESA_VIDEO_800x600p_60Hz_4_3,
    HDMI_EDID_VESA_VIDEO_800x600p_72Hz_4_3,
    HDMI_EDID_VESA_VIDEO_800x600p_75Hz_4_3,
    HDMI_EDID_VESA_VIDEO_800x600p_85Hz_4_3,
    HDMI_EDID_VESA_VIDEO_848x480p_60Hz_4_3,
    HDMI_EDID_VESA_VIDEO_1024x768p_43Hz_4_3,
    HDMI_EDID_VESA_VIDEO_1024x768p_60Hz_4_3,
    HDMI_EDID_VESA_VIDEO_1024x768p_70Hz_4_3,
    HDMI_EDID_VESA_VIDEO_1024x768p_75Hz_4_3,
    HDMI_EDID_VESA_VIDEO_1024x768p_85Hz_4_3,
    HDMI_EDID_VESA_VIDEO_1152x864p_75Hz_4_3,
    HDMI_EDID_VESA_VIDEO_1280x768p_60Hz_CVT_16_9,
    HDMI_EDID_VESA_VIDEO_1280x768p_60Hz_16_9,
    HDMI_EDID_VESA_VIDEO_1280x768p_75Hz_16_9,
    HDMI_EDID_VESA_VIDEO_1280x768p_85Hz_16_9,
    HDMI_EDID_VESA_VIDEO_1280x800p_60Hz_CVT_16_9,
    HDMI_EDID_VESA_VIDEO_1280x800p_60Hz_16_9,
    HDMI_EDID_VESA_VIDEO_1280x800p_75Hz_16_9,
    HDMI_EDID_VESA_VIDEO_1280x800p_85Hz_16_9,
    HDMI_EDID_VESA_VIDEO_1280x960p_60Hz_4_3,
    HDMI_EDID_VESA_VIDEO_1280x960p_85Hz_4_3,
    HDMI_EDID_VESA_VIDEO_1280x1024p_60Hz_4_3,
    HDMI_EDID_VESA_VIDEO_1280x1024p_75Hz_4_3,
    HDMI_EDID_VESA_VIDEO_1280x1024p_85Hz_4_3,
    HDMI_EDID_VESA_VIDEO_1360x768p_60Hz_16_9,
    HDMI_EDID_VESA_VIDEO_1400x1050p_60Hz_CVT_4_3,
    HDMI_EDID_VESA_VIDEO_1400x1050p_60Hz_4_3,
    HDMI_EDID_VESA_VIDEO_1400x1050p_75Hz_4_3,
    HDMI_EDID_VESA_VIDEO_1400x1050p_85Hz_4_3,
    HDMI_EDID_VESA_VIDEO_1440x900p_60Hz_CVT_16_9,
    HDMI_EDID_VESA_VIDEO_1440x900p_60Hz_16_9,
    HDMI_EDID_VESA_VIDEO_1440x900p_75Hz_16_9,
    HDMI_EDID_VESA_VIDEO_1440x900p_85Hz_16_9,
    HDMI_EDID_VESA_VIDEO_1600x1200p_60Hz_4_3,
    HDMI_EDID_VESA_VIDEO_1600x1200p_65Hz_4_3,
    HDMI_EDID_VESA_VIDEO_1600x1200p_70Hz_4_3,
    HDMI_EDID_VESA_VIDEO_1600x1200p_75Hz_4_3,
    HDMI_EDID_VESA_VIDEO_1600x1200p_80Hz_4_3,
    HDMI_EDID_VESA_VIDEO_1680x1050p_60Hz_CVT_16_9,
    HDMI_EDID_VESA_VIDEO_1680x1050p_60Hz_16_9,
    HDMI_EDID_VESA_VIDEO_1680x1050p_75Hz_16_9,
    HDMI_EDID_VESA_VIDEO_1680x1050p_85Hz_16_9,
    HDMI_EDID_VESA_VIDEO_1920x1080p_60Hz_16_9
};

//#if DEBUG_LEVEL
/*3d supported format of sink*/
static char *s3d_format[] = {
        "HDMI_S3D_FRAME_PACKING",
        "HDMI_S3D_FIELD_ALTERNATIVE",
        "HDMI_S3D_LINE_ALTERNATIVE",
        "HDMI_S3D_SIDE_BY_SIDE_FULL",
        "HDMI_S3D_L_DEPTH",
        "HDMI_S3D_L_DEPTH_GRAPHICS_GRAPHICS_DEPTH",
        "HDMI_S3D_SIDE_BY_SIDE_HALF_FORMAT"
};
//#endif
#define PIXEL_CLOCK_OFFSET             500
static void edid_get_ext_timings_info(int current_descriptor_addrs, u8 *edid ,
                                      hdmi_video_timings *timings);
static int edid_get_timings_info(hdmi_edid_dtd *edid_dtd,
                                 hdmi_video_timings *timings);
static hdmi_cm get_timing_code(hdmi_video_timings *timing);

/******************************************************************************
* Function:       edid_get_timing_order
* Description:    get timing order from timings list.
* Data Accessed:
* Data Updated:
* Input:          timing code
* Output:         order in list by priority
* Return:         video timing
* Others:
*******************************************************************************/
int edid_get_timing_order(int code, bool mhl_check)
{
    int count = ARRAY_SIZE(timing_order);
    int i = 0;
    int max_order = count;

    if (mhl_check && hw_support_mhl()) {
        max_order = MHL_NOT_SUPPORT_TIMING_ORDER;
    }

    for (i = 0; i < count; i++) {
        if (timing_order[i] == code) {
            if(i >= max_order){
                return -1;
            }
            return i;
        }
    }
    return -1;
}

int edid_get_vesa_timing_order(int code, bool mhl_check)
{
    int count = ARRAY_SIZE(vesa_timing_order);
    int i = 0;

    for (i = 0; i < count; i++) {
        if (vesa_timing_order[i] == code) {
            return i;
        }
    }
    return -1;
}

/******************************************************************************
* Function:       edid_get_timing
* Description:    get video timing by timing index
* Data Accessed:
* Data Updated:
* Input:          index of video timing
* Output:
* Return:         video timing
* Others:
*******************************************************************************/
#if HDMI_USE_NOW
static hdmi_video_timings *edid_get_timing(int index)
{
    BUG_ON((index < 0) || (index >= ARRAY_SIZE(all_timings_direct)));

    return all_timings_direct + index;
}
#endif
/******************************************************************************
* Function:       get_datablock_offset
* Description:    get offset of cea block by extension_edid_db
* Data Accessed:
* Data Updated:
* Input:          edid data and data block type of cea
* Output:         offset index
* Return:
* Others:
*******************************************************************************/
static int get_datablock_offset(u8 *edid, extension_edid_db datablock,
                                int *offset)
{
    int current_byte = 0;
    int disp = 0;
    int i = 0;
    int number = 0;

    BUG_ON((NULL == edid) || (NULL == offset));

    if (0x00 == edid[0x7e]) {
        logi( "has NO extension block! \n");
        return -1;
    }

    disp = edid[(0x80) + 2];
    logd( "Extension block present db %d %x\n", datablock, disp);
    if (0x4 == disp) {
        loge( "NO data is provided in the reserved data block! \n");
        return -1;
    }

    i = 0x80 + 0x4;
    logd( "%x\n", i);
    while (i < (0x80 + disp)) {
        current_byte = edid[i];

        logd( "i = %d current_byte = 0x%x (current_byte & HDMI_EDID_EX_DATABLOCK_TAG_MASK) = %d\n",
              i, current_byte, (current_byte & HDMI_EDID_EX_DATABLOCK_TAG_MASK));

        if ((current_byte >> 5) == datablock) {
            *offset = i;
            logd( "datablock %d %d\n", datablock, *offset);

            return 0;

        } else {
            number = (current_byte & HDMI_EDID_EX_DATABLOCK_LEN_MASK) + 1;
            i += number;
        }
    }
    loge( "not found Extension block db %d \n", datablock);
    return -1;
}

/******************************************************************************
* Function:       edid_get_timing_mode
* Description:    differentiate timing code between interleaved(I) and pronounced(P)
* Data Accessed:
* Data Updated:
* Input:          timing code
* Output:
* Return:         I or P in edid_timing_mode
* Others:
*******************************************************************************/
edid_timing_mode edid_get_timing_mode(int code)
{
    switch (code) {
        case HDMI_EDID_EX_VIDEO_640x480p_60Hz_4_3    :
        case HDMI_EDID_EX_VIDEO_720x480p_60Hz_4_3    :
        case HDMI_EDID_EX_VIDEO_720x480p_60Hz_16_9   :
        case HDMI_EDID_EX_VIDEO_1280x720p_60Hz_16_9  :
        case HDMI_EDID_EX_VIDEO_720x240p_60Hz_4_3    :
        case HDMI_EDID_EX_VIDEO_720x240p_60Hz_16_9   :
        case HDMI_EDID_EX_VIDEO_2880x240p_60Hz_4_3   :
        case HDMI_EDID_EX_VIDEO_2880x240p_60Hz_16_9  :
        case HDMI_EDID_EX_VIDEO_1440x480p_60Hz_4_3   :
        case HDMI_EDID_EX_VIDEO_1440x480p_60Hz_16_9  :
        case HDMI_EDID_EX_VIDEO_720x576p_50Hz_4_3    :
        case HDMI_EDID_EX_VIDEO_720x576p_50Hz_16_9   :
        case HDMI_EDID_EX_VIDEO_1280x720p_50Hz_16_9  :
        case HDMI_EDID_EX_VIDEO_720x288p_50Hz_4_3    :
        case HDMI_EDID_EX_VIDEO_720x288p_50Hz_16_9   :
        case HDMI_EDID_EX_VIDEO_1440x576p_50Hz_4_3   :
        case HDMI_EDID_EX_VIDEO_1440x576p_50Hz_16_9  :
        case HDMI_EDID_EX_VIDEO_1920x1080p_24Hz_16_9 :
        case HDMI_EDID_EX_VIDEO_1920x1080p_25Hz_16_9 :
        case HDMI_EDID_EX_VIDEO_1920x1080p_30Hz_16_9 :
        case HDMI_EDID_EX_VIDEO_1920x1080p_60Hz_16_9 :
        case HDMI_EDID_EX_VIDEO_1920x1080p_50Hz_16_9 :{
            return EDID_TIMING_MODE_P;
        }

        default: {
            return EDID_TIMING_MODE_I;
        }
    }
}

/******************************************************************************
* Function:       edid_get_video_svds
* Description:    get offset of timing code block from cea
* Data Accessed:
* Data Updated:
* Input:          edid data
* Output:         offset of timing code block and timing number
* Return:
* Others:
*******************************************************************************/
static void edid_get_video_svds(u8 *edid, int *offset, int *svd_number)
{
    extension_edid_db vdb =  DATABLOCK_VIDEO;

    BUG_ON(NULL == edid);
    BUG_ON((NULL == offset) || (NULL == svd_number));

    if (!get_datablock_offset(edid, vdb, offset)) {
        *svd_number = edid[*offset] & HDMI_EDID_EX_DATABLOCK_LEN_MASK;
        (*offset)++;

        logi("there is a video timing info.\n");
        return;
    }

    logw("there isn't a video timing info.\n");
    *svd_number = 0;
    *offset = 0;

    return;
}

/******************************************************************************
* Function:       edid_get_timing_code
* Description:    get video code from edid
* Data Accessed:
* Data Updated:
* Input:          parsed data of edid, s3d enabled
* Output:
* Return:         hdmi mode and code
* Others:
*******************************************************************************/
hdmi_cm edid_get_timing_code(hdmi_edid *edid, bool s3d_enabled)
{
    int i        = 0;
    int j        = 0;
    u32 offset   = 0;
    u32 addr     = 0;

    int svd_base    = 0;
    int svd_number  = 0;
    int svd_code    = 0;
    int svd_native  = 0;

    bool audio_support = false;

    hdmi_cm cm = {INVALID_VALUE, INVALID_VALUE};

    hdmi_video_timings edid_timings = {0};

    BUG_ON(NULL == edid);

#if ENABLE_EDID_FAULT_TOLERANCE
    if (false == edid_is_valid_edid((u8*)edid)) {
        loge("the edid isn't readed, and use default code.\n");
        cm.mode = HDMI_DEFAULT_TIMING_MODE;
        cm.code = edid_get_default_code();
        return cm;
    }
#endif
    /*
     *  Verify if the sink supports audio
     */
    /* check if EDID has CEA extension block */
    if ((edid->extension_edid != 0x00)
        /* check if CEA extension block is version 3 */
        && (3 == edid->extention_rev)
        /* check if extension block has the IEEE HDMI ID*/
        &&(edid_has_ieee_id((u8 *)edid))
        /* check if sink supports basic audio */
        &&(edid->num_dtd & HDMI_AUDIO_BASIC_MASK)) {

        audio_support = true;
    }

    /* Seach block 0, there are 4 DTDs arranged in priority order */
    for (i = 0; i < EDID_SIZE_BLOCK0_TIMING_DESCRIPTOR; i++) {
        if (0 == edid_get_timings_info(&edid->dtd[i], &edid_timings)) {
            logi("this block[%d] hasn't timing info.\n", i);
            continue;
        }

        logd("Block0 [%d] timings:\n", i);
        edid_dump_video_timings(&edid_timings);
        cm = get_timing_code(&edid_timings);
        logd("Block0 [%d] value matches code = %d , mode = %d\n", i, cm.code, cm.mode);

        if (INVALID_VALUE == cm.code) {
            logi("this block[%d]'s code is invalid.\n", i);
            continue;
        }

        if ((s3d_enabled)
            && (INVALID_VALUE == s3d_code_cea[cm.code])) {
            logi("this block[%d]'s code isn't s3d.\n", i);
            continue;
        }

        /* if sink supports audio, use CEA video timing */
        if ((audio_support) && (HDMI_CODE_TYPE_VESA == cm.mode)) {
            logi("this block[%d]'s mode don't surport audio.\n", i);
            audio_support = false;
            //continue;
        }
        return cm;
    }

    /* Search SVDs in block 1 twice: first natives and then all */
    if (0x00 != edid->extension_edid) {
        edid_get_video_svds((u8 *)edid, &svd_base, &svd_number);
        for (j = 1; j >= 0; j--) {
            for (i = 0; i < svd_number; i++) {
                svd_native = ((u8 *)edid)[svd_base+i] & HDMI_EDID_EX_VIDEO_NATIVE;
                svd_code = ((u8 *)edid)[svd_base+i] & HDMI_EDID_EX_VIDEO_MASK;

                if (svd_code >= ARRAY_SIZE(code_cea)) {
                    continue;
                }

                /* Check if this SVD is native*/
                if ((!svd_native) && j) {
                    continue;
                }

                /* Check if this 3D CEA timing is supported*/
                if ((s3d_enabled) && (INVALID_VALUE == s3d_code_cea[svd_code])) {
                    continue;
                }

                /* Check if this CEA timing is supported*/
                if (INVALID_VALUE == code_cea[svd_code]) {
                    continue;
                }

                return cm;
            }
        }
    }

    /* Search DTDs in block1 */
    if (edid->extension_edid != 0x00) {
        offset = edid->offset_dtd;
        if (offset != 0) {
            addr = EDID_DESCRIPTOR_BLOCK1_ADDRESS + offset;
        }

        for (i = 0; i < EDID_SIZE_BLOCK1_TIMING_DESCRIPTOR; i++) {
            edid_get_ext_timings_info(addr, (u8 *)edid, &edid_timings);
            addr += EDID_TIMING_DESCRIPTOR_SIZE;
            cm = get_timing_code(&edid_timings);
            logd("Block1[%d] value matches code = %d , mode = %d\n",
                 i, cm.code, cm.mode);
            if (INVALID_VALUE == cm.code) {
                continue;
            }

            if ((true == s3d_enabled) && (INVALID_VALUE == s3d_code_cea[cm.code])) {
                continue;
            }

            /* if sink supports audio, use CEA video timing */
            if ((true == audio_support) && (HDMI_CODE_TYPE_VESA == cm.mode)) {
                continue;
            }

            return cm;
        }
    }

    /*As last resort, check for best standard timing supported:*/
    if (edid->timing_1 & 0x01) {
        logd("800x600@60Hz\n");
        cm.mode = HDMI_CODE_TYPE_VESA;
        cm.code = HDMI_EDID_VESA_VIDEO_800x600p_60Hz_4_3;

        return cm;
    }

    if (edid->timing_2 & 0x08) {
        logd("1024x768@60Hz\n");
        cm.mode = HDMI_CODE_TYPE_VESA;
        cm.code = HDMI_EDID_VESA_VIDEO_1024x768p_60Hz_4_3;

        return cm;
    }

    cm.mode = INVALID_VALUE;
    cm.code = INVALID_VALUE;

    return cm;
}

/******************************************************************************
* Function:       edid_get_best_timing
* Description:    get best video code from edid
* Data Accessed:
* Data Updated:
* Input:          parsed data of edid
* Output:
* Return:         hdmi mode and code
* Others:
*******************************************************************************/
hdmi_cm edid_get_best_timing(hdmi_edid *edid)
{
    hdmi_cm cm = {INVALID_VALUE, INVALID_VALUE};

#if !HDMI_USE_DEFAULT_TIMING
    int has_image_format = 0;
    int i = 0;
    image_format img_format = {0};
    hdmi_cm externcm = {INVALID_VALUE, INVALID_VALUE};
    int order = 0;
    int best_order = 0;
#endif

    IN_FUNCTION;

#if HDMI_USE_DEFAULT_TIMING //Use default timing
    cm.mode = HDMI_DEFAULT_TIMING_MODE;
    cm.code = HDMI_DEFAULT_TIMING_CODE;
    logi("for fpga test, get best code:%d.\n", cm.code);
    return cm;
#else

#if HDCP_FOR_CERTIFICATION
    //always return 480P
    cm.code = HDCP_DEFAULT_TIMING_CODE;
    cm.mode = HDMI_CODE_TYPE_CEA;
    logi("for hdcp test, return code:%d.\n", cm.code);
    return cm;
#endif

#if ENABLE_EDID_FAULT_TOLERANCE
    if (false == edid_is_valid_edid((u8*)edid)) {
        loge("the edid isn't readed, and use default code.\n");
        cm.mode = HDMI_DEFAULT_TIMING_MODE;
        cm.code = edid_get_default_code();
        OUT_FUNCTION;
        return cm;
    }
#endif

    /*get all support timing code*/
    has_image_format = edid_get_image_format((u8 *)edid, &img_format);
    if (!has_image_format) {
        logw("there isn't video image format in edid.\n");
    }

    logd(" video format number: %d.\n", img_format.number);

    for (i = 0 ; i < img_format.number; i++) {
        order = edid_get_timing_order(img_format.fmt[i].code, true);
        if (order >= best_order) {
            best_order = order;
            cm.mode = HDMI_CODE_TYPE_CEA;
            cm.code = img_format.fmt[i].code;
        }
    }

    if (0 == best_order) {
        externcm = edid_get_timing_code((hdmi_edid *)edid, false);
        if (HDMI_CODE_TYPE_CEA == externcm.mode) {
            order = edid_get_timing_order(externcm.code, true);
            if (order) {
                cm.code = externcm.code;
                cm.mode = externcm.mode;
            }
        } else {
            if (INVALID_VALUE != externcm.code) {
                externcm.code = externcm.code + HDMI_CODE_VESA_OFFSET;

                if (externcm.code > HDMI_MAX_VESA_TIMING_CODE) {
                    externcm.code = HDMI_DEFAULT_VESA_TIMING_CODE;
                }

                order = edid_get_vesa_timing_order(externcm.code, true);
                if (order >= best_order) {
                    cm.code = externcm.code;
                    cm.mode = externcm.mode;
                }
            }
        }
    }

    OUT_FUNCTION;
    return cm;

#endif

}

/******************************************************************************
* Function:       edid_get_default_code
* Description:
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*******************************************************************************/
int edid_get_default_code()
{
    if (hw_support_mhl()) {
        return HDMI_DEFAULT_MHL_TIMING_CODE;
    }
#if HDCP_FOR_CERTIFICATION
    return HDCP_DEFAULT_TIMING_CODE;
#endif
    return HDMI_DEFAULT_TIMING_CODE;;
}

/******************************************************************************
* Function:       edid_get_av_delay
* Description:    get latency of audio and video from edid
* Data Accessed:
* Data Updated:
* Input:          edid data
* Output:         struct of latency
* Return:
* Others:
*******************************************************************************/
static void edid_get_av_delay(u8 *edid, latency *lat)
{
    int offset       = 0;
    int current_byte = 0;
    int number       = 0;
    extension_edid_db vsdb =  DATABLOCK_VENDOR;

    BUG_ON((NULL == edid) || (NULL == lat));

    if (!get_datablock_offset(edid, vsdb, &offset)) {
        current_byte = edid[offset];
        number = current_byte & HDMI_EDID_EX_DATABLOCK_LEN_MASK;

        if ((number >= 8) && ((current_byte + 8) & 0x80)) {
            lat->vid_latency = ((current_byte + 8) - 1) * 2;
            lat->aud_latency = ((current_byte + 9) - 1) * 2;
        }

        if ((number >= 8) && ((current_byte + 8) & 0xC0)) {
            lat->int_vid_latency = ((current_byte + 10) - 1) * 2;
            lat->int_aud_latency = ((current_byte + 11) - 1) * 2;
        }
    }

    return;
}
/******************************************************************************
* Function:       dump_s3d_format
* Description:    dump the s3d supported format info of sink
* Data Accessed:
* Data Updated:
* Input:          edid offset
* Output:         3d supported format
* Return:
* Others:
*******************************************************************************/
void s3d_format_dump(u8 *edid,int offset){
    u8 *e = NULL;
    int i = 0;
    e = edid;
    e += offset;

    for(; i < 6; i++){
        if(FLD_GET(e[0],i,i)){
            logd( "s3d_surpported_format:%s\n" ,s3d_format[i]);
        }
    }

    e += 1;
    if(FLD_GET(e[0],0,0)){
        logd( "s3d_surpported_format:%s\n" ,s3d_format[6]);
    }
}
/******************************************************************************
* Function:       edid_s3d_supported
* Description:    get 3D ability of the device from edid
* Data Accessed:
* Data Updated:
* Input:          edid data
* Output:
* Return:         whether the device support 3D
* Others:
*******************************************************************************/
bool edid_s3d_supported(u8 *edid)
{
    bool s3d_support  = false;
    int  offset       = 0;
    int  current_byte = 0;
    int  current_bit = 0;

    BUG_ON(NULL == edid);

    if (!get_datablock_offset(edid, DATABLOCK_VENDOR, &offset)) {
       if((edid[offset] & 0x1F) < 8){
           s3d_support =false;
       }else {
            offset += 8;
            current_byte = edid[offset++];
            /* Latency_Fields_Present */
            if (current_byte & 0x80) {
                offset += 2;
            }

            /* I_Latency_Fields_Present */
            if (current_byte & 0x40) {
                offset += 2;
            }

            /* HDMI_Video_present */
            if (current_byte & 0x20) {//HDMI_video_present
                current_byte = edid[offset];
                /* 3D_Present */
                if (current_byte & 0x80) {
                    logi( "S3D supported\n");
                    s3d_support = true;

                }
                if(s3d_support){
                   current_bit = FLD_GET(current_byte,6,5);
                   if((current_bit == 0x01) || (current_bit == 0x02)){
                       offset += 1;//HMDI_VIC_LEN
                       current_byte = edid[offset++];
                       offset += current_byte >> 5;
                       s3d_format_dump(edid,offset);
                   }
                }

            }
       }
    }

    return s3d_support;
}

bool edid_cec_getphyaddr(u8 *edid, u8* physical_addr)
{
    bool result       = false;
    int  offset       = 0;

    BUG_ON(NULL == edid);
    BUG_ON(NULL == physical_addr);

    if (!get_datablock_offset(edid, DATABLOCK_VENDOR, &offset)) {
       if((edid[offset] & 0x1F) < 5){//8 is bug
           result = false;
       } else {
            offset += 4;
            physical_addr[0] = (edid[offset] & 0xF0)>>4;
            physical_addr[1] = (edid[offset] & 0x0F);
            physical_addr[2] = (edid[offset+1] & 0xF0)>>4;
            physical_addr[3] = (edid[offset+1] & 0x0F);
            result = true;
       }
    }
    return result;
}

/******************************************************************************
* Function:       edid_get_s3d_timings_index
* Description:    get 3D timing code
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:         timing code of 3D format
* Others:
*******************************************************************************/
int edid_get_s3d_timings_index(int mode, int code)
{
    int index = s3d_code_cea[code];

    if (INVALID_VALUE == index) {
        index = code_cea[edid_get_default_code()];
    }

    return index;
}

/******************************************************************************
* Function:       edid_get_timings_index
* Description:    get index of video timing from timing code
* Data Accessed:
* Data Updated:
* Input:          timing code and timing mode
* Output:
* Return:         timing index
* Others:
*******************************************************************************/
static int get_timings_index(int mode, int code)
{
    int index = INVALID_VALUE;

    if (edid_is_valid_code(mode, code)) {
        if (HDMI_CODE_TYPE_VESA == mode) {
            index = code_vesa[code];
        } else {
            index = code_cea[code];
        }
    }

    return index;
}

/******************************************************************************
* Function:       edid_get_timings_index
* Description:    get index of timing by code and mode
* Data Accessed:
* Data Updated:
* Input:          timing code and timing mode
* Output:
* Return:         timing index
* Others:
*******************************************************************************/
int edid_get_timings_index(int mode, int code)
{
    hdmi_video_timings *timing = NULL;
    int x_res   = 0;
    int y_res   = 0;
    int tmp     = 0;

    int index = get_timings_index(mode, code);

    if (INVALID_VALUE != index) {
        timing = edid_get_timings_byindex(index);
        x_res = timing->x_res;
        y_res = timing->y_res;
        if (x_res > y_res) {
            tmp = x_res;
            x_res = y_res;
            y_res = tmp;
        }
    }
    if (INVALID_VALUE == index
        || x_res > K3_FB_MAX_WIDTH
        || y_res > K3_FB_MAX_HEIGHT) {
        index =  INVALID_VALUE;
    }
    return index;
}

/******************************************************************************
* Function:       edid_get_timings_info
* Description:
* Data Accessed:
* Data Updated:
* Input:          dtd of edid
* Output:         video timing
* Return:         1 - there is timing info  0 - there isn't timing info
* Others:
*******************************************************************************/
static int edid_get_timings_info(hdmi_edid_dtd *edid_dtd,
                                 hdmi_video_timings *timings)
{
    BUG_ON((NULL == edid_dtd) || (NULL == timings));

    if (edid_dtd->video.pixel_clock) {
        hdmi_edid_dtd_video *vid = &edid_dtd->video;

        timings->pixel_clock = 10 * vid->pixel_clock;
        timings->x_res = vid->horiz_active | (((u16)vid->horiz_high & 0xf0) << 4);
        timings->y_res = vid->vert_active | (((u16)vid->vert_high & 0xf0) << 4);

        timings->hfp = vid->horiz_sync_offset | (((u16)vid->sync_pulse_high & 0xc0) << 2);
        timings->hsw = vid->horiz_sync_pulse | (((u16)vid->sync_pulse_high & 0x30) << 4);
        timings->hbp = (vid->horiz_blanking |
                        (((u16)vid->horiz_high & 0x0f) << 8)) -
                       (timings->hfp + timings->hsw);

        timings->vfp = ((vid->vert_sync_pulse & 0xf0) >> 4) | ((vid->sync_pulse_high & 0x0f) << 2);
        timings->vsw = (vid->vert_sync_pulse & 0x0f) | ((vid->sync_pulse_high & 0x03) << 4);
        timings->vbp = (vid->vert_blanking |
                        (((u16)vid->vert_high & 0x0f) << 8)) -
                       (timings->vfp + timings->vsw);

        logd("there is timing info.\n");
        return 1;
    }

    switch (edid_dtd->monitor_name.block_type) {
        case HDMI_EDID_DTD_TAG_STANDARD_TIMING_DATA:
            logi( "standard timing data, there isn't timing info.\n");
            break;
        case HDMI_EDID_DTD_TAG_COLOR_POINT_DATA:
            logi( "color point data, there isn't timing info.\n");
            break;
        case HDMI_EDID_DTD_TAG_MONITOR_NAME:
            logi( "monitor name: %s, , there isn't timing info.\n", edid_dtd->monitor_name.text);
            break;
        case HDMI_EDID_DTD_TAG_MONITOR_LIMITS:
            logi( "this is monitor limits.\n");
            break;
        case HDMI_EDID_DTD_TAG_ASCII_STRING:
            logi( "this is ascii string, and there isn't timing info.\n");
            break;
        case HDMI_EDID_DTD_TAG_MONITOR_SERIALNUM:
            logi( "monitor serialnum: %s, there isn't timing info.\n", edid_dtd->monitor_serial_number.text);
            break;
        default:
            logi( "unsupported EDID descriptor block format, there isn't timing info.\n");
            break;
    }

    return 0;
}

/******************************************************************************
* Function:       edid_get_ext_timings_info
* Description:    get video timing from current address of edid data
* Data Accessed:
* Data Updated:
* Input:          edid and current address
* Output:         video timing
* Return:
* Others:
*******************************************************************************/
static void edid_get_ext_timings_info(int current_descriptor_addrs, u8 *edid ,
                                      hdmi_video_timings *timings)
{
    BUG_ON((NULL == edid) || (NULL == timings));

    timings->x_res = (((edid[current_descriptor_addrs + 4] & 0xF0) << 4)
                      | edid[current_descriptor_addrs + 2]);
    timings->y_res = (((edid[current_descriptor_addrs + 7] & 0xF0) << 4)
                      | edid[current_descriptor_addrs + 5]);

    timings->pixel_clock = ((edid[current_descriptor_addrs + 1] << 8)
                            | edid[current_descriptor_addrs]);
    timings->pixel_clock = 10 * timings->pixel_clock;

    timings->hfp = edid[current_descriptor_addrs + 8];
    timings->hsw = edid[current_descriptor_addrs + 9];
    timings->hbp = (((edid[current_descriptor_addrs + 4] & 0x0F) << 8)
                    | edid[current_descriptor_addrs + 3]) -
                   (timings->hfp + timings->hsw);

    timings->vfp = ((edid[current_descriptor_addrs + 10] & 0xF0) >> 4);
    timings->vsw = (edid[current_descriptor_addrs + 10] & 0x0F);
    timings->vbp = (((edid[current_descriptor_addrs + 7] & 0x0F) << 8)
                    | edid[current_descriptor_addrs + 6]) -
                   (timings->vfp + timings->vsw);

    return;
}

/******************************************************************************
* Function:       get_timing_code
* Description:    get timing code from video timing
* Data Accessed:
* Data Updated:
* Input:          video timing
* Output:
* Return:         timing code and timing mode
* Others:
*******************************************************************************/
static hdmi_cm get_timing_code(hdmi_video_timings *timings)
{
    int i            = 0;
    int temp_vsync   = 0;
    int temp_hsync   = 0;
    int timing_hsync = 0;
    int timing_vsync = 0;
    int code         = INVALID_VALUE;

    hdmi_video_timings temp = {0};
    hdmi_cm cm = {INVALID_VALUE, INVALID_VALUE};

    BUG_ON(NULL == timings);

    logd("get_timing_code\n");

    for (i = 0; i < ARRAY_SIZE(all_timings_direct); i++) {
        temp = all_timings_direct[i];
        if ((((temp.pixel_clock + PIXEL_CLOCK_OFFSET) < timings->pixel_clock)
            || ((temp.pixel_clock - PIXEL_CLOCK_OFFSET) > timings->pixel_clock))
            || (temp.x_res != timings->x_res)
            || (temp.y_res != timings->y_res)) {

            continue;
        }

        temp_hsync = temp.hfp + temp.hsw + temp.hbp;
        timing_hsync = timings->hfp + timings->hsw + timings->hbp;
        temp_vsync = temp.vfp + temp.vsw + temp.vbp;
        timing_vsync = timings->vfp + timings->vsw + timings->vbp;

        logd("Temp_hsync = %d, temp_vsync = %d, "
             "timing_hsync = %d, timing_vsync = %d\n",
             temp_hsync, temp_vsync, timing_hsync, timing_vsync);

        if ((temp_hsync == timing_hsync) && (temp_vsync == timing_vsync)) {
            code = i;
            cm.code = code_index[i];
            cm.mode = (code < HDMI_TIMINGS_VESA_START || code > HDMI_TIMINGS_VESA_END) ? HDMI_CODE_TYPE_CEA : HDMI_CODE_TYPE_VESA;
            logd("Video code = %d mode = %s\n", cm.code, cm.mode ? "CEA" : "VESA");
            break;
        }
    }
    return cm;
}

/******************************************************************************
* Function:       edid_get_image_format
* Description:    get image format which the device support from edid
* Data Accessed:
* Data Updated:
* Input:          edid data
* Output:         struct of image format
* Return:         1 -- there is image format  0 -- there isn't image format
* Others:
*******************************************************************************/
int edid_get_image_format(u8 *edid, image_format *format)
{
    int offset       = 0;
    int current_byte = 0;
    int i            = 0;
    int j            = 0;
    int number       = 0;
    extension_edid_db vsdb =  DATABLOCK_VIDEO;

    BUG_ON((NULL == edid) || (NULL == format));

    format->number = 0;
    memset(format->fmt, 0, sizeof(format->fmt));

    if (0 != get_datablock_offset(edid, vsdb, &offset)) {
        logi("there isn't video datablock.\n");
        return 0;
    }

    current_byte = edid[offset];
    number = current_byte & HDMI_EDID_EX_DATABLOCK_LEN_MASK;

#if ENABLE_MOCK_24FPS
    format->fmt[0].code = 32;
    format->fmt[1].code = 33;
    format->fmt[2].code = 34;
    i = 3;
    number -= i;
#endif

    for (j = 1 ; j <= number ; j++) {
        current_byte = edid[offset+j];
        format->fmt[i].code = current_byte & HDMI_EDID_EX_VIDEO_MASK;
        format->fmt[i].pref = current_byte & HDMI_EDID_EX_VIDEO_NATIVE;
        if (++i == HDMI_IMG_FORMAT_MAX_LENGTH) {
            break;
        }
    }
    format->number = i;

    return 1;
}

/******************************************************************************
* Function:       edid_get_audio_format
* Description:    get image format which the device support from edid
* Data Accessed:
* Data Updated:
* Input:          edid data
* Output:         struct of audio format
* Return:         1 - there is audio format, 0 - there isn't audio format
* Others:
*******************************************************************************/
int edid_get_audio_format(u8 *edid, audio_format *format)
{
    int offset       = 0;
    int current_byte = 0;
    int j            = 0;
    int number       = 0;
    int ind          = 0;
    extension_edid_db vsdb =  DATABLOCK_AUDIO;

    BUG_ON((NULL == edid) || (NULL == format));

    format->number = 0;
    memset(format->fmt, 0, sizeof(format->fmt));

    if (!get_datablock_offset(edid, vsdb, &offset)) {
        logd("edid_get_audio_format  offset == %02x \n",offset);
        current_byte = edid[offset];
        number = current_byte & HDMI_EDID_EX_DATABLOCK_LEN_MASK;

        if (number/3 >= HDMI_AUDIO_FORMAT_MAX_LENGTH) {
            format->number = HDMI_AUDIO_FORMAT_MAX_LENGTH;
        } else {
            format->number = number/3;
        }

        for (j = 1 ; j < number ; j++) {
            if (1 == j%3) {
                current_byte = edid[offset + j];
                if (ind >= HDMI_AUDIO_FORMAT_MAX_LENGTH ) {
                    logw("get wrang ind, return :%d", 1);
                    return 1;
                }
                format->fmt[ind].format = FLD_GET(current_byte, 6, 3);
                format->fmt[ind].num_of_ch = FLD_GET(current_byte, 2, 0) + 1;
            } else if (2 == j%3) {
                format->fmt[ind].sampling_freq = edid[offset + j];
                ind++;
            }
        }

        return 1;
    }

    return 0;
}

/******************************************************************************
* Function:       edid_has_vcdb
* Description:    get whether the device have vcdb
* Data Accessed:
* Data Updated:
* Input:          edid data
* Output:
* Return:         whether the device have vcdb OUI
* Others:
*******************************************************************************/
bool edid_has_vcdb(u8 *edid)
{
    int offset       = 0;
    int current_byte = 0;
    extension_edid_db vsdb =  DATABLOCK_EXTENDED_TAG;

    BUG_ON(NULL == edid);
    IN_FUNCTION;

    if (!get_datablock_offset(edid, vsdb, &offset)) {
        current_byte = edid[offset+1];

        if (0 == current_byte) {
            logi("there is video capability data block.\n");
            return true;
        }
    }
    OUT_FUNCTION;
    return false;
}

/******************************************************************************
* Function:       edid_has_ieee_id
* Description:    get whether the device have IEEE registration OUI
* Data Accessed:
* Data Updated:
* Input:          edid data
* Output:
* Return:         whether the device have IEEE registration OUI
* Others:
*******************************************************************************/
bool edid_has_ieee_id(u8 *edid)
{
    int offset       = 0;
    int current_byte = 0;
    int number       = 0;
    extension_edid_db vsdb =  DATABLOCK_VENDOR;
    u32 hdmi_identifier = 0;

    BUG_ON(NULL == edid);

    if (!get_datablock_offset(edid, vsdb, &offset)) {
        current_byte = edid[offset];
        number = current_byte & HDMI_EDID_EX_DATABLOCK_LEN_MASK;

        if (number < 3) {
            logd("number < 3, and hasn't ieee id.\n");
            return false;
        }

        offset++;
        hdmi_identifier = edid[offset] | edid[offset+1]<<8
                          | edid[offset+2]<<16;
        if (HDMI_IEEE_REGISTRATION_ID == hdmi_identifier) {
            logd("there is a ieee id.\n");
            return true;
        }
    }

    return false;
}

/******************************************************************************
* Function:       edid_get_deep_color_info
* Description:    get deep color ability of the device
* Data Accessed:
* Data Updated:
* Input:          edid data
* Output:         struct of deep_color
* Return:
* Others:
*******************************************************************************/
void edid_get_deep_color_info(u8 *edid, deep_color *format)
{
    int offset       = 0;
    int current_byte = 0;
    int number       = 0;
    extension_edid_db vsdb = DATABLOCK_VENDOR;

    BUG_ON((NULL == edid) || (NULL == format));

    if (!get_datablock_offset(edid, vsdb, &offset)) {
        if ((offset < 0) || (offset >= HDMI_EDID_MAX_LENGTH)) {
            logw("get wrang offset :%d", offset);
            return;
        }
        current_byte = edid[offset];
        number = current_byte & HDMI_EDID_EX_DATABLOCK_LEN_MASK;

        if (number >= 6) {
            format->bit_30 = ((current_byte + 6) & 0x10);
            format->bit_36 = ((current_byte + 6) & 0x20);
        }

        if (number >= 7) {
            format->max_tmds_freq = ((current_byte + 7)) * 5;
        }
    }

    return;
}

/******************************************************************************
* Function:       edid_tv_yuv_supported
* Description:    get whether the device support YUV
* Data Accessed:
* Data Updated:
* Input:          edid data
* Output:
* Return:         whether the device support YUV
* Others:
*******************************************************************************/
bool edid_tv_yuv_supported(u8 *edid)
{
    BUG_ON(NULL == edid);

    if ((edid[0x7e] != 0x00) && (edid[0x83] & 0x30)) {
        logi( "YUV supported\n");
        return true;
    }

    return false;
}
//#if USE_PP_MODE
/******************************************************************************
* Function:       edid_supported_yuv422
* Description:    get whether the device support YUV422
* Data Accessed:
* Data Updated:
* Input:          edid data
* Output:
* Return:         whether the device support YUV422
* Others:
*******************************************************************************/
bool edid_supported_yuv422(u8 *edid)
{
    BUG_ON(NULL == edid);
    if ((edid[0x7e] != 0x00) && (edid[0x83] & 0x10)) {
        logi( "YUV422 supported\n");
        return true;
    }

    return false;
}
//#endif
/******************************************************************************
* Function:       edid_ai_supported
* Description:    get whether the device support ai
* Data Accessed:
* Data Updated:
* Input:          edid data
* Output:
* Return:         whether the device support AI
* Others:
*******************************************************************************/
bool edid_ai_supported(u8 *edid)
{
    int offset       = 0;
    int current_byte = 0;
    int number       = 0;

    BUG_ON(NULL == edid);

    if (!get_datablock_offset(edid, DATABLOCK_VENDOR, &offset)) {
        current_byte = edid[offset];
        number = current_byte & HDMI_EDID_EX_DATABLOCK_LEN_MASK;
        if (number < 6) {
            return false;
        }

        offset += 6;
        if (edid[offset] & HDMI_EDID_EX_SUPPORTS_AI_MASK) {
            return true;
        }
    }

    return false;
}

/******************************************************************************
* Function:       edid_dump
* Description:    print edid data
* Data Accessed:
* Data Updated:
* Input:          edid data
* Output:
* Return:
* Others:
*******************************************************************************/
void edid_dump(u8* edid)
{
#if DEBUG_LEVEL
    int i = 0;

    BUG_ON(NULL == edid);

    printk("\n========================== edid dump ==========================\n");
    for (; i < HDMI_EDID_MAX_LENGTH; i++) {
        if ((i%16) == 0) {
            printk("\n%02x :",(i/16));
        }
        /* debug func for dump edid, use printk for easy read format. */
        printk(" %02x",edid[i]);
    }
    printk("\n========================== edid dump ==========================\n");
#endif
    return;
}

/******************************************************************************
* Function:       edid_dump_parser
* Description:    print parsed edid data
* Data Accessed:
* Data Updated:
* Input:          edid data
* Output:
* Return:
* Others:
*******************************************************************************/
void edid_dump_parser(u8* edid)
{
    u32 i      = 0;
    int offset = 0;
    int addr   = 0;
    int number = 0;
    int has_timing_info = 0;
    int ret    = 0;
#if DEBUG_LEVEL
    u8  *e     = NULL;
#endif
    hdmi_edid    *edid_st     = (hdmi_edid *)edid;
    image_format img_format  = {0};
    audio_format aud_format  = {0};
    deep_color   vsdb_format = {0};
    latency      lat         = {0};

    hdmi_video_timings timings = {0};
    hdmi_cm cm = {INVALID_VALUE, INVALID_VALUE};

    BUG_ON(NULL == edid);

#if DEBUG_LEVEL

    printk("\n=========================== edid parse ===========================\n");
    e = edid;
    printk( "\nedid_header:\n%02x\t%02x\t%02x\t%02x\t%02x\t%02x\t"
            "%02x\t%02x\n", e[0], e[1], e[2], e[3], e[4], e[5], e[6], e[7]);
    e += 8;
    printk( "Vendor & Product:\n%02x\t%02x\t%02x\t%02x\t%02x\t"
            "%02x\t%02x\t%02x\t%02x\t%02x\n",
            e[0], e[1], e[2], e[3], e[4], e[5], e[6], e[7], e[8], e[9]);
    e += 10;
    printk( "EDID Structure:\n%02x\t%02x\n",
            e[0], e[1]);
    e += 2;
    printk( "Basic Display Parameter:\n%02x\t%02x\t%02x\t%02x\t%02x\n",
            e[0], e[1], e[2], e[3], e[4]);
    e += 5;
    printk( "Color Characteristics:\n%02x\t%02x\t%02x\t%02x\t"
            "%02x\t%02x\t%02x\t%02x\t%02x\t%02x\n",
            e[0], e[1], e[2], e[3], e[4], e[5], e[6], e[7], e[8], e[9]);
    e += 10;
    printk( "Established timings:\n%02x\t%02x\t%02x\n",
            e[0], e[1], e[2]);
    e += 3;
    printk( "Standard timings:\n%02x\t%02x\t%02x\t%02x\t%02x\t"
            "%02x\t%02x\t%02x\n",
            e[0], e[1], e[2], e[3], e[4], e[5], e[6], e[7]);
    e += 8;
    printk( "%02x\t%02x\t%02x\t%02x\t%02x\t%02x\t%02x\t%02x\n",
            e[0], e[1], e[2], e[3], e[4], e[5], e[6], e[7]);
    e += 8;
#endif

    for (i = 0; i < EDID_SIZE_BLOCK0_TIMING_DESCRIPTOR; i++) {
        logd( "Extension 0 Block %d\n", i);
        has_timing_info = edid_get_timings_info(&edid_st->dtd[i],
                                               &timings);
        if (has_timing_info) {
            cm = get_timing_code(&timings);
            edid_dump_video_timings(&timings);
            logd( "Video code: %d video mode %d\n", cm.code, cm.mode);
        }
    }

    if (edid[0x7e] != 0x00) {
        offset = edid[EDID_DESCRIPTOR_BLOCK1_ADDRESS + 2];
        logd( "offset %x\n", offset);
        if (offset != 0) {
            addr = EDID_DESCRIPTOR_BLOCK1_ADDRESS + offset;
            /* to determine the number of descriptor blocks */
            for (i = 0; i < EDID_SIZE_BLOCK1_TIMING_DESCRIPTOR;
                 i++) {
                logd( "Extension 1 Block %d\n", i);
                edid_get_ext_timings_info(addr, edid, &timings);
                addr += EDID_TIMING_DESCRIPTOR_SIZE;
                edid_dump_video_timings(&timings);
                cm = get_timing_code(&timings);
                logd( "Video code: %d video mode %d\n",
                        cm.code, cm.mode);
            }
        }
        edid_get_video_svds(edid, &offset, &number);
        logd( "No of SVDs: %d\n", number);
        for (i = 0; i < number; i++) {
            logd( "SVD[%d]: CEA code[%d], native[%s]\n",
                    i, edid[offset+i] & HDMI_EDID_EX_VIDEO_MASK,
                    edid[offset+i] & HDMI_EDID_EX_VIDEO_NATIVE ?
                    "YES" : "NO");
        }
        logd( "No. of native dtd: %d\n",
                (edid[EDID_DESCRIPTOR_BLOCK1_ADDRESS + 3]
                 & HDMI_VIDEO_NATIVE_DTDS_MASK));

        logd( "Supports basic audio: %s\n",
                (edid[EDID_DESCRIPTOR_BLOCK1_ADDRESS + 3]
                 & HDMI_AUDIO_BASIC_MASK) ? "YES" : "NO");
    }

    logd( "Has IEEE HDMI ID: %s\n", edid_has_ieee_id(edid) ? "YES" : "NO");

    ret = edid_get_image_format(edid, &img_format);
    logd( "%d image number\n", img_format.number);
    for (i = 0 ; (ret && i < img_format.number); i++) {
        logi( "%d %d pref code\n",
                img_format.fmt[i].pref, img_format.fmt[i].code);
    }

    ret = edid_get_audio_format(edid, &aud_format);
    logd( "%d audio number\n", aud_format.number);
    for (i = 0 ; (ret && i < aud_format.number); i++) {
        logi( "%d %d format num_of_channels\n",
                aud_format.fmt[i].format,
                aud_format.fmt[i].num_of_ch);
    }

    edid_get_deep_color_info(edid, &vsdb_format);
    logi( "%d deep color bit 30 %d  deep color 36 bit "
            "%d max tmds freq\n", vsdb_format.bit_30, vsdb_format.bit_36,
            vsdb_format.max_tmds_freq);

    edid_get_av_delay(edid, &lat);
    logi( "%d vid_latency %d aud_latency "
            "%d interlaced vid latency %d interlaced aud latency\n",
            lat.vid_latency, lat.aud_latency,
            lat.int_vid_latency, lat.int_aud_latency);

    logd( "YUV supported %d\n", (int)edid_tv_yuv_supported(edid));
    logd("\n=========================== edid parse ===========================\n");

    return;
}

/******************************************************************************
* Function:       edid_dump_video_timings
* Description:    debug function to print video timing
* Data Accessed:
* Data Updated:
* Input:          video timings
* Output:
* Return:
* Others:
*******************************************************************************/
void edid_dump_video_timings(hdmi_video_timings *timings)
{
#if DEBUG_LEVEL

    BUG_ON(NULL == timings);

    printk( "Timing Info:\n"
            "  pixel_clk = %d\n"
            "  x_res     = %d\n"
            "  y_res     = %d\n"
            "  hfp       = %d\n"
            "  hsw       = %d\n"
            "  hbp       = %d\n"
            "  vfp       = %d\n"
            "  vsw       = %d\n"
            "  vbp       = %d\n",
            timings->pixel_clock,
            timings->x_res,
            timings->y_res,
            timings->hfp,
            timings->hsw,
            timings->hbp,
            timings->vfp,
            timings->vsw,
            timings->vbp);
#endif
    return;
}

/******************************************************************************
* Function:       edid_read
* Description:    get edid data from the device and parse video timing
* Data Accessed:
* Data Updated:
* Input:          edid data
* Output:         video timing
* Return:
* Others:
*******************************************************************************/
int edid_read(u8* edid)
{
    int ret   = 0;

    BUG_ON(NULL == edid);

    IN_FUNCTION;

    ret = hw_core_read_edid(edid, HDMI_EDID_MAX_LENGTH);
    if (ret) {
        loge( "HDMI failed to read E-EDID\n");
        return ret;
    }

    edid_dump(edid);

    if (false == edid_is_valid_edid(edid)) {
        loge("the edid header isn't matching.\n");
        OUT_FUNCTION;
        return -1;
    }

    //edid_dump_parser(edid);

    OUT_FUNCTION;
    return ret;
}

/******************************************************************************
* Function:       edid_get_timings_byindex
* Description:    get video timing by timing code
* Data Accessed:
* Data Updated:
* Input:          code of video timing
* Output:
* Return:         video timing
* Others:
*******************************************************************************/
hdmi_video_timings* edid_get_timings_byindex(int index)
{
    BUG_ON((index < 0) || (index >= ARRAY_SIZE(all_timings_direct)));
    return &all_timings_direct[index];
}

/******************************************************************************
* Function:       edid_get_hvpol_bycode
* Description:    get Vsync and Hsync polarity by code
* Data Accessed:
* Data Updated:
* Input:          timing code
* Output:         hdmi_hvsync_pol of Vsync and Hsync polarity
* Return:
* Others:
*******************************************************************************/
hdmi_hvsync_pol* edid_get_hvpol_byindex(int index)
{
    BUG_ON((index < 0) || (index >= ARRAY_SIZE(hvpol_mapping)));
    return &hvpol_mapping[index];
}

/******************************************************************************
* Function:       edid_is_valid_code
* Description:    get whether the code is valid
* Data Accessed:
* Data Updated:
* Input:          timing mode and timing code
* Output:
* Return:         whether the code is valid
* Others:
*******************************************************************************/
bool edid_is_valid_code(int mode, int code)
{
    if (code < 0) {
        return false;
    }
    return (((HDMI_CODE_TYPE_VESA == mode) && (code < ARRAY_SIZE(code_vesa)) && (code_vesa[code] >= 0))
            || ((HDMI_CODE_TYPE_CEA == mode) && (code < ARRAY_SIZE(code_cea)) && (code_cea[code] >= 0))) ;
}

/******************************************************************************
* Function:       edid_is_valid_edid
* Description:
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*******************************************************************************/
bool edid_is_valid_edid(u8 *edid)
{
    if(memcmp(edid, edid_header, sizeof(edid_header))) {
        return false;
    }

    return true;
}

