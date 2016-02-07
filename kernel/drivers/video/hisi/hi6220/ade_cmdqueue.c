/* Copyright (c) 2008-2010, Hisilicon Tech. Co., Ltd. All rights reserved.
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

#include <linux/fb.h>
#include <linux/clk.h>
#include <linux/regulator/consumer.h>
#include <linux/delay.h>
#include <trace/trace_kernel.h>
#include <linux/iommu.h>

#include "balong_fb.h"
#include "ade_reg_offset.h"
#include "ade_union_define.h"
#include "ade_cmdqueue.h"

#include <linux/fs.h>
#include <asm/uaccess.h>

char str_cmd_file[2048] = {0};
int  *debug_cmd_vaddr = NULL;
int  debug_str_len = 0;
static int file_count = 0;
static int scl_flag = 0;

struct write_cmd_head  wr_cmd;

const s32  scl_mitchell[] = {
    0, 114, 1820, 114, 0, 0,
    0, 106, 1819, 122, 0, 0,
    0, 98, 1819, 130, 0,  0,
    0, 91, 1817, 139, 0,  0,
    0, 83, 1816, 149, -1, 0,
    0, 76, 1813, 158, -1, 0,
    0, 69, 1811, 168, -1, 0,
    0, 63, 1808, 178, -2, 0,
    0, 56, 1804, 189, -2, 0,
    0, 50, 1800, 200, -3, 0,
    0, 44, 1796, 211, -4, 0,
    0, 38, 1791, 223, -5, 0,
    0, 32, 1786, 235, -5, 0,
    0, 27, 1780, 247, -6, 0,
    0, 21, 1774, 259, -7, 0,
    0, 16, 1767, 272, -8, 0,
    0, 11, 1760, 285, -9, 0,
    0, 6, 1753, 298, -10, 0,
    0, 1, 1745, 312, -11, 0,
    0, -3, 1737, 325, -12, 0,
    0, -8, 1729, 339, -14, 0,
    0, -12, 1720, 354, -15, 0,
    0, -16, 1711, 368, -16, 0,
    0, -20, 1701, 383, -17, 0,
    0, -23, 1691, 398, -19, 0,
    0, -27, 1681, 413, -20, 0,
    0, -30, 1671, 428, -21, 0,
    0, -34, 1660, 444, -23, 0,
    0, -37, 1649, 460, -24, 0,
    0, -40, 1637, 475, -26, 0,
    0, -43, 1625, 492, -27, 0,
    0, -45, 1613, 508, -29, 0,
    0, -48, 1601, 524, -30, 0,
    0, -50, 1588, 541, -32, 0,
    0, -53, 1575, 557, -33, 0,
    0, -55, 1562, 574, -35, 0,
    0, -57, 1549, 591, -36, 0,
    0, -59, 1535, 608, -38, 0,
    0, -61, 1521, 626, -39, 0,
    0, -62, 1507, 643, -41, 0,
    0, -64, 1493, 661, -42, 0,
    0, -65, 1478, 678, -44, 0,
    0, -67, 1463, 696, -45, 0,
    0, -68, 1448, 714, -47, 0,
    0, -69, 1433, 731, -48, 0,
    0, -70, 1417, 749, -50, 0,
    0, -71, 1402, 767, -51, 0,
    0, -72, 1386, 785, -53, 0,
    0, -72, 1370, 803, -54, 0,
    0, -73, 1354, 822, -55, 0,
    0, -73, 1337, 840, -57, 0,
    0, -74, 1321, 858, -58, 0,
    0, -74, 1304, 876, -59, 0,
    0, -74, 1287, 894, -60, 0,
    0, -74, 1270, 913, -62, 0,
    0, -74, 1253, 931, -63, 0,
    0, -74, 1236, 949, -64, 0,
    0, -74, 1219, 967, -65, 0,
    0, -74, 1201, 986, -66, 0,
    0, -74, 1184, 1004, -67, 0,
    0, -73, 1166, 1022, -68, 0,
    0, -73, 1148, 1040, -69, 0,
    0, -72, 1130, 1058, -70, 0,
    0, -72, 1113, 1077, -70, 0,
    0, -71, 1095, 1095, -71, 0,
};

const u32   scl_blk_rot[390] = {
    0, 0, 0, 2047, 0, 0,
    0, 0, 0, 2047, 0, 0,
    0, 0, 0, 2047, 0, 0,
    0, 0, 0, 2047, 0, 0,
    0, 0, 0, 2047, 0, 0,
    0, 0, 0, 2047, 0, 0,
    0, 0, 0, 2047, 0, 0,
    0, 0, 0, 2047, 0, 0,
    0, 0, 0, 2047, 0, 0,
    0, 0, 0, 2047, 0, 0,
    0, 0, 0, 2047, 0, 0,
    0, 0, 0, 2047, 0, 0,
    0, 0, 0, 2047, 0, 0,
    0, 0, 0, 2047, 0, 0,
    0, 0, 0, 2047, 0, 0,
    0, 0, 0, 2047, 0, 0,
    0, 0, 0, 2047, 0, 0,
    0, 0, 0, 2047, 0, 0,
    0, 0, 0, 2047, 0, 0,
    0, 0, 0, 2047, 0, 0,
    0, 0, 0, 2047, 0, 0,
    0, 0, 0, 2047, 0, 0,
    0, 0, 0, 2047, 0, 0,
    0, 0, 0, 2047, 0, 0,
    0, 0, 0, 2047, 0, 0,
    0, 0, 0, 2047, 0, 0,
    0, 0, 0, 2047, 0, 0,
    0, 0, 0, 2047, 0, 0,
    0, 0, 0, 2047, 0, 0,
    0, 0, 0, 2047, 0, 0,
    0, 0, 0, 2047, 0, 0,
    0, 0, 0, 2047, 0, 0,
    0, 0, 2047, 0, 0, 0,
    0, 0, 2047, 0, 0, 0,
    0, 0, 2047, 0, 0, 0,
    0, 0, 2047, 0, 0, 0,
    0, 0, 2047, 0, 0, 0,
    0, 0, 2047, 0, 0, 0,
    0, 0, 2047, 0, 0, 0,
    0, 0, 2047, 0, 0, 0,
    0, 0, 2047, 0, 0, 0,
    0, 0, 2047, 0, 0, 0,
    0, 0, 2047, 0, 0, 0,
    0, 0, 2047, 0, 0, 0,
    0, 0, 2047, 0, 0, 0,
    0, 0, 2047, 0, 0, 0,
    0, 0, 2047, 0, 0, 0,
    0, 0, 2047, 0, 0, 0,
    0, 0, 2047, 0, 0, 0,
    0, 0, 2047, 0, 0, 0,
    0, 0, 2047, 0, 0, 0,
    0, 0, 2047, 0, 0, 0,
    0, 0, 2047, 0, 0, 0,
    0, 0, 2047, 0, 0, 0,
    0, 0, 2047, 0, 0, 0,
    0, 0, 2047, 0, 0, 0,
    0, 0, 2047, 0, 0, 0,
    0, 0, 2047, 0, 0, 0,
    0, 0, 2047, 0, 0, 0,
    0, 0, 2047, 0, 0, 0,
    0, 0, 2047, 0, 0, 0,
    0, 0, 2047, 0, 0, 0,
    0, 0, 2047, 0, 0, 0,
    0, 0, 2047, 0, 0, 0,
    0, 0, 2047, 0, 0, 0
};

const s32   scl_lanczos2[390] = {
    0, 0x0,   0x7ff,  0x0  , 0x0,    0,
    0, 0xff6, 0x7ff,  0xa  , 0x0,    0,
    0, 0xfec, 0x7fe,  0x15 , 0x0,    0,
    0, 0xfe3, 0x7fc,  0x20 , 0x0,    0,
    0, 0xfda, 0x7fa,  0x2b , 0xfff,  0,
    0, 0xfd1, 0x7f8,  0x37 , 0xfff,  0,
    0, 0xfc9, 0x7f5,  0x43 , 0xfff,  0,
    0, 0xfc1, 0x7f1,  0x4f , 0xffe,  0,
    0, 0xfb9, 0x7ed,  0x5c , 0xffe,  0,
    0, 0xfb1, 0x7e8,  0x68 , 0xffd,  0,
    0, 0xfaa, 0x7e3,  0x76 , 0xffd,  0,
    0, 0xfa3, 0x7dd,  0x83 , 0xffc,  0,
    0, 0xf9d, 0x7d6,  0x91 , 0xffb,  0,
    0, 0xf96, 0x7d0,  0x9f , 0xffa,  0,
    0, 0xf90, 0x7c8,  0xad , 0xff9,  0,
    0, 0xf8b, 0x7c0,  0xbc , 0xff8,  0,
    0, 0xf85, 0x7b8,  0xcb , 0xff7,  0,
    0, 0xf80, 0x7af,  0xda , 0xff6,  0,
    0, 0xf7c, 0x7a5,  0xe9 , 0xff5,  0,
    0, 0xf77, 0x79b,  0xf9 , 0xff3,  0,
    0, 0xf73, 0x791,  0x109, 0xff2,  0,
    0, 0xf6f, 0x786,  0x119, 0xff1,  0,
    0, 0xf6b, 0x77b,  0x12a, 0xfef,  0,
    0, 0xf68, 0x76f,  0x13b, 0xfed,  0,
    0, 0xf65, 0x763,  0x14c, 0xfec,  0,
    0, 0xf62, 0x756,  0x15d, 0xfea,  0,
    0, 0xf5f, 0x749,  0x16f, 0xfe8,  0,
    0, 0xf5d, 0x73b,  0x181, 0xfe6,  0,
    0, 0xf5b, 0x72e,  0x193, 0xfe4,  0,
    0, 0xf59, 0x71f,  0x1a5, 0xfe2,  0,
    0, 0xf57, 0x711,  0x1b7, 0xfe0,  0,
    0, 0xf56, 0x701,  0x1ca, 0xfde,  0,
    0, 0xf54, 0x6f2,  0x1dd, 0xfdc,  0,
    0, 0xf53, 0x6e2,  0x1f0, 0xfd9,  0,
    0, 0xf52, 0x6d2,  0x203, 0xfd7,  0,
    0, 0xf52, 0x6c2,  0x217, 0xfd5,  0,
    0, 0xf51, 0x6b1,  0x22b, 0xfd2,  0,
    0, 0xf51, 0x6a0,  0x23e, 0xfd0,  0,
    0, 0xf51, 0x68e,  0x252, 0xfcd,  0,
    0, 0xf51, 0x67d,  0x267, 0xfca,  0,
    0, 0xf52, 0x66b,  0x27b, 0xfc8,  0,
    0, 0xf52, 0x658,  0x28f, 0xfc5,  0,
    0, 0xf53, 0x646,  0x2a4, 0xfc2,  0,
    0, 0xf54, 0x633,  0x2b9, 0xfbf,  0,
    0, 0xf55, 0x620,  0x2ce, 0xfbc,  0,
    0, 0xf56, 0x60d,  0x2e3, 0xfba,  0,
    0, 0xf57, 0x5f9,  0x2f8, 0xfb7,  0,
    0, 0xf59, 0x5e5,  0x30d, 0xfb4,  0,
    0, 0xf5a, 0x5d2,  0x323, 0xfb1,  0,
    0, 0xf5c, 0x5bd,  0x338, 0xfae,  0,
    0, 0xf5e, 0x5a9,  0x34e, 0xfab,  0,
    0, 0xf60, 0x595,  0x363, 0xfa8,  0,
    0, 0xf62, 0x580,  0x379, 0xfa5,  0,
    0, 0xf64, 0x56b,  0x38f, 0xfa1,  0,
    0, 0xf66, 0x556,  0x3a4, 0xf9e,  0,
    0, 0xf68, 0x541,  0x3ba, 0xf9b,  0,
    0, 0xf6b, 0x52c,  0x3d0, 0xf98,  0,
    0, 0xf6d, 0x517,  0x3e6, 0xf95,  0,
    0, 0xf70, 0x501,  0x3fc, 0xf92,  0,
    0, 0xf72, 0x4ec,  0x412, 0xf8f,  0,
    0, 0xf75, 0x4d6,  0x428, 0xf8c,  0,
    0, 0xf78, 0x4c1,  0x43e, 0xf89,  0,
    0, 0xf7a, 0x4ab,  0x454, 0xf86,  0,
    0, 0xf7d, 0x495,  0x46a, 0xf83,  0,
    0, 0xf80, 0x47f,  0x47f, 0xf80,  0,
};

const s32  scl_lanczos2_gaussian10[] = {
    0, 609, 830, 609, 0, 0,
    -3, 605, 830, 613, 3, 0,
    -6, 600, 830, 617, 6, 0,
    -9, 596, 829, 621, 10, 0,
    -11, 592, 830, 625, 12, 0,
    -14, 587, 829, 629, 16, 0,
    -16, 583, 829, 632, 20, 0,
    -19, 579, 829, 636, 23, -1,
    -21, 574, 828, 640, 27, -1,
    -23, 570, 828, 643, 30, -1,
    -26, 565, 828, 647, 34, -1,
    -28, 561, 827, 650, 37, -1,
    -29, 556, 827, 654, 41, -1,
    -32, 552, 827, 657, 45, -2,
    -33, 547, 826, 661, 49, -2,
    -35, 542, 825, 664, 53, -2,
    -37, 538, 825, 667, 57, -3,
    -38, 533, 824, 670, 61, -3,
    -39, 528, 823, 673, 65, -3,
    -41, 524, 823, 676, 69, -4,
    -42, 519, 822, 679, 73, -4,
    -43, 514, 821, 682, 77, -4,
    -44, 509, 821, 685, 82, -5,
    -45, 504, 820, 688, 86, -6,
    -46, 499, 819, 691, 91, -6,
    -47, 494, 818, 693, 95, -7,
    -48, 489, 817, 696, 99, -7,
    -48, 484, 816, 699, 104, -8,
    -49, 479, 815, 701, 108, -8,
    -50, 474, 815, 704, 113, -9,
    -50, 469, 814, 706, 118, -10,
    -51, 464, 812, 709, 122, -10,
    -51, 459, 811, 711, 127, -11,
    -51, 454, 811, 714, 132, -12,
    -52, 449, 810, 716, 137, -12,
    -52, 444, 808, 718, 142, -13,
    -52, 438, 807, 721, 146, -14,
    -52, 433, 806, 723, 151, -14,
    -52, 428, 805, 725, 156, -15,
    -52, 423, 804, 727, 161, -16,
    -52, 418, 803, 729, 166, -17,
    -52, 412, 802, 731, 171, -18,
    -51, 407, 801, 733, 176, -18,
    -51, 402, 799, 735, 181, -19,
    -51, 397, 798, 737, 186, -20,
    -51, 391, 797, 739, 191, -21,
    -50, 386, 796, 741, 196, -22,
    -50, 381, 794, 743, 201, -23,
    -49, 376, 793, 745, 207, -23,
    -49, 370, 792, 746, 212, -24,
    -48, 365, 790, 748, 217, -25,
    -48, 360, 789, 750, 222, -26,
    -47, 354, 788, 752, 227, -27,
    -46, 349, 787, 753, 232, -28,
    -46, 344, 785, 755, 237, -29,
    -45, 338, 784, 757, 243, -30,
    -44, 333, 783, 758, 248, -31,
    -44, 328, 781, 760, 253, -32,
    -43, 322, 780, 762, 259, -33,
    -42, 317, 778, 763, 264, -34,
    -41, 312, 777, 765, 269, -34,
    -40, 307, 775, 766, 275, -35,
    -40, 301, 774, 768, 280, -36,
    -39, 296, 773, 770, 285, -37,
    -38, 290, 771, 771, 290, -38,
};

const s32   scl_lanczos2_gaussian40[] = {
    0, 679, 690, 679, 0, 0,
    -3, 675, 690, 682, 3, 0,
    -7, 672, 690, 685, 7, 0,
    -10, 668, 690, 688, 11, 0,
    -13, 665, 690, 691, 14, 0,
    -16, 661, 690, 695, 18, 0,
    -18, 656, 690, 697, 22, 0,
    -21, 653, 690, 700, 26, -1,
    -24, 648, 690, 703, 30, -1,
    -26, 645, 691, 705, 33, -1,
    -28, 640, 691, 708, 38, -1,
    -31, 636, 691, 710, 42, -1,
    -33, 632, 691, 712, 46, -2,
    -35, 627, 691, 715, 51, -2,
    -37, 623, 692, 717, 55, -2,
    -39, 618, 692, 718, 60, -3,
    -41, 614, 692, 721, 64, -3,
    -42, 609, 693, 722, 69, -3,
    -44, 604, 693, 724, 74, -4,
    -45, 600, 693, 725, 78, -4,
    -47, 595, 694, 727, 83, -5,
    -48, 590, 694, 728, 88, -5,
    -49, 585, 695, 730, 93, -6,
    -50, 580, 695, 731, 98, -6,
    -51, 574, 695, 732, 103, -7,
    -52, 569, 696, 733, 108, -7,
    -53, 564, 697, 734, 114, -8,
    -54, 559, 697, 735, 119, -9,
    -55, 554, 698, 736, 124, -9,
    -55, 548, 698, 736, 129, -10,
    -56, 543, 699, 737, 135, -11,
    -56, 537, 700, 737, 140, -11,
    -57, 532, 700, 738, 146, -12,
    -57, 526, 701, 739, 151, -13,
    -58, 520, 702, 739, 157, -14,
    -58, 515, 702, 739, 163, -14,
    -58, 509, 703, 740, 169, -15,
    -58, 503, 704, 740, 174, -16,
    -58, 498, 705, 740, 180, -17,
    -58, 492, 706, 740, 186, -18,
    -58, 486, 706, 740, 192, -19,
    -58, 480, 707, 740, 197, -20,
    -57, 474, 708, 740, 203, -21,
    -57, 468, 709, 739, 209, -22,
    -57, 462, 710, 739, 215, -23,
    -56, 456, 710, 739, 221, -23,
    -56, 450, 711, 739, 227, -24,
    -55, 444, 712, 738, 233, -25,
    -55, 438, 713, 738, 239, -26,
    -54, 432, 714, 737, 246, -27,
    -54, 426, 715, 737, 252, -28,
    -53, 420, 716, 736, 258, -29,
    -52, 413, 716, 736, 264, -30,
    -52, 407, 718, 735, 270, -31,
    -51, 401, 719, 735, 276, -33,
    -50, 395, 719, 734, 282, -34,
    -49, 389, 720, 733, 289, -34,
    -49, 382, 721, 733, 295, -35,
    -48, 376, 722, 732, 301, -36,
    -47, 370, 723, 731, 307, -37,
    -46, 364, 724, 730, 314, -38,
    -45, 357, 725, 730, 320, -39,
    -44, 351, 726, 729, 326, -40,
    -43, 345, 726, 728, 333, -41,
    -42, 339, 727, 727, 339, -42,
};

const s32 scl_lanczos3[390] = {
    0, 0, 2047, 0, 0, 0,
    3, -13, 2047, 13, -3, 0,
    6, -26, 2046, 27, -7, 0,
    10, -38, 2045, 41, -10, 0,
    13, -51, 2043, 55, -14, 0,
    15, -62, 2042, 70, -18, 0,
    18, -74, 2039, 84, -21, 1,
    21, -85, 2036, 100, -25, 1,
    24, -96, 2033, 115, -29, 1,
    26, -107, 2029, 130, -33, 1,
    29, -118, 2025, 146, -37, 1,
    31, -128, 2020, 162, -41, 2,
    34, -138, 2015, 179, -45, 2,
    36, -147, 2010, 196, -50, 2,
    38, -156, 2004, 212, -54, 3,
    40, -165, 1998, 230, -58, 3,
    42, -174, 1991, 247, -63, 4,
    44, -182, 1984, 265, -67, 4,
    46, -190, 1976, 282, -72, 5,
    47, -198, 1968, 301, -76, 5,
    49, -206, 1960, 319, -81, 6,
    50, -213, 1951, 337, -86, 7,
    52, -220, 1942, 356, -90, 7,
    53, -226, 1932, 375, -95, 8,
    54, -233, 1922, 394, -100, 9,
    56, -239, 1912, 414, -105, 9,
    57, -244, 1901, 433, -110, 10,
    58, -250, 1889, 453, -114, 11,
    59, -255, 1878, 473, -119, 12,
    60, -260, 1866, 493, -124, 12,
    60, -264, 1853, 514, -129, 13,
    61, -269, 1841, 534, -134, 14,
    62, -273, 1828, 555, -139, 15,
    62, -277, 1814, 576, -144, 16,
    63, -280, 1800, 596, -149, 17,
    63, -283, 1786, 618, -154, 18,
    63, -286, 1771, 639, -159, 19,
    64, -289, 1757, 660, -164, 20,
    64, -292, 1741, 681, -169, 21,
    64, -294, 1726, 703, -174, 22,
    64, -296, 1710, 725, -179, 23,
    64, -297, 1694, 746, -184, 24,
    64, -299, 1677, 768, -189, 25,
    64, -300, 1660, 790, -193, 26,
    64, -301, 1643, 812, -198, 28,
    63, -302, 1626, 834, -203, 29,
    63, -303, 1608, 856, -208, 30,
    63, -303, 1590, 878, -212, 31,
    62, -303, 1572, 900, -217, 32,
    62, -303, 1554, 922, -221, 33,
    61, -303, 1535, 945, -226, 34,
    61, -302, 1516, 967, -230, 36,
    60, -301, 1497, 989, -235, 37,
    60, -300, 1477, 1011, -239, 38,
    59, -299, 1458, 1033, -243, 39,
    58, -298, 1438, 1055, -247, 40,
    57, -296, 1418, 1077, -251, 41,
    57, -295, 1398, 1099, -255, 42,
    56, -293, 1377, 1121, -258, 44,
    55, -291, 1357, 1143, -262, 45,
    54, -289, 1336, 1165, -265, 46,
    53, -286, 1315, 1187, -269, 47,
    52, -284, 1294, 1209, -272, 48,
    51, -281, 1273, 1230, -275, 49,
    50, -278, 1252, 1252, -278, 50,
};
/*
u32 csc_601_narrow_rgb2yuv[10] = {
    0x99, 0x12d, 0x3a,
    0x1FA8,0x1F52, 0x103,
    0x103, 0x1F25, 0x1fd6,
    0x0
};
u32 dc_601_narrow_rgb2yuv[12] = {
    0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x80,
    0x0,0x0,0x0,0x80
};
*/
u32 csc_601_wide_rgb2yuv[10] = {
    0x84,0x102,0x32,
    0x1FB4,0x1F6B,0xE1,
    0xE1,0x1F44,0x1FDC,
    0x0
};
u32 dc_601_wide_rgb2yuv[12] = {
    0x0,0x0,0x0,0x10,
    0x0,0x0,0x0,0x80,
    0x0,0x0,0x0,0x80
};

// yuv2rgb config
u32 csc_601_narrow_yuv2rgb[10] = {
    0x200, 0x0, 0x2BE,
    0x200, 0x1F54, 0x1E9F,
    0x200, 0x377, 0x0,
    0x0
};
u32 dc_601_narrow_yuv2rgb[12] = {
    0x0, 0x0, 0x180, 0x0,
    0x0, 0x180, 0x180, 0x0,
    0x0, 0x180, 0x0, 0x0
};

u32 csc_601_wide_yuv2rgb[10] = {
    0x0254,0x0,0x0331,
    0x0254,0x1F38,0x1E60,
    0x0254,0x0409,0x0,
    0x0
};
u32 dc_601_wide_yuv2rgb[12] = {
    0x1F0,0x0,0x180,0x0,
    0x1F0,0x180,0x180,0x0,
    0x1F0,0x180,0x0,0x0
};

u32 csc_709_narrow_yuv2rgb[10] = {
    0x200, 0x0, 0x314,
    0x200, 0x1FA2, 0x1F15,
    0x200, 0x3A2, 0x0,
    0x0
};
u32 dc_709_narrow_yuv2rgb[12] = {
    0x0, 0x0, 0x180, 0x0,
    0x0, 0x180, 0x180, 0x0,
    0x0, 0x180, 0x0, 0x0
};

u32 csc_709_wide_yuv2rgb[10] = {
    0x254, 0x0, 0x396,
    0x254, 0x1F93, 0x1EEA,
    0x254, 0x43B, 0x0,
    0x0
};
u32 dc_709_wide_yuv2rgb[12] = {
    0x1F0,0x0,0x180,0x0,
    0x1F0,0x180,0x180,0x0,
    0x1F0,0x180,0x0,0x0
};

bool g_is_set_color_temperature = false ;
struct semaphore g_ct_sem;
static u32  dc_601_wide_rgb2rgb[12] = {
    0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0
};
 u32  g_601_csc_value[10] = {
    0x100,0x0,0x0,
    0x0,0x100,0x0,
    0x0,0x0,0x100,
    0x0
};
SCL_CONFIG_COEF scl_coef[3];

static bool isAlphaRGBType(int format)
{
    switch (format) {
    //case ADE_XRGB_8888:
    //case ADE_XBGR_8888:
    case ADE_ARGB_8888:
    case ADE_ABGR_8888:
    case ADE_RGBA_8888:
    case ADE_BGRA_8888:
        return true;
    default:
        return false;
    }
    return false;
}


static u32 *getCtranYuv2rgbCscArray(u32 yuv_info, int *len)
{
    switch (yuv_info) {
        case ADE_YUV_601_WIDE:
            *len = sizeof(csc_601_wide_yuv2rgb) / sizeof(u32);
            return csc_601_wide_yuv2rgb;
        case ADE_YUV_601_NARROW:
            *len = sizeof(csc_601_narrow_yuv2rgb) / sizeof(u32);
            return csc_601_narrow_yuv2rgb;
        case ADE_YUV_709_WIDE:
            *len = sizeof(csc_709_wide_yuv2rgb) / sizeof(u32);
            return csc_709_wide_yuv2rgb;
        case ADE_YUV_709_NARROW:
            *len = sizeof(csc_709_narrow_yuv2rgb) / sizeof(u32);
            return csc_709_narrow_yuv2rgb;
        default:
            *len = sizeof(csc_601_wide_yuv2rgb) / sizeof(u32);
            return csc_601_wide_yuv2rgb;
    }
}

static u32 *getCtranYuv2rgbChdcArray(u32 yuv_info, int *len)
{
    switch (yuv_info) {
        case ADE_YUV_601_WIDE:
            *len = sizeof(dc_601_wide_yuv2rgb) / sizeof(u32);
            return dc_601_wide_yuv2rgb;
        case ADE_YUV_601_NARROW:
            *len = sizeof(dc_601_narrow_yuv2rgb) / sizeof(u32);
            return dc_601_narrow_yuv2rgb;
        case ADE_YUV_709_WIDE:
            *len = sizeof(dc_709_wide_yuv2rgb) / sizeof(u32);
            return dc_709_wide_yuv2rgb;
        case ADE_YUV_709_NARROW:
            *len = sizeof(dc_709_narrow_yuv2rgb) / sizeof(u32);
            return dc_709_narrow_yuv2rgb;
        default:
            *len = sizeof(dc_601_wide_yuv2rgb) / sizeof(u32);
            return dc_601_wide_yuv2rgb;
    }
}

static int getRdmaRot2RotAngle(int rot)
{
    /* ADE_ROT_90: clockwise,
     * ADE_ROT_ANGLE_270: anti-clockwise
     */
    switch (rot) {
        case ADE_ROT_NOP:
            return ADE_ROT_ANGLE_NOP;
        case ADE_ROT_90:
            return ADE_ROT_ANGLE_90;
        case ADE_ROT_180:
            return ADE_ROT_ANGLE_180;
        case ADE_ROT_270:
            return ADE_ROT_ANGLE_270;
        case ADE_ROT_H_MIRROR:
            return ADE_ROT_ANGLE_H_MIRROR;
        case ADE_ROT_V_MIRROR:
            return ADE_ROT_ANGLE_V_MIRROR;
        case ADE_ROT_90_H_MIRROR:
            return ADE_ROT_ANGLE_90_H_MIRROR;
        case ADE_ROT_90_V_MIRROR:
            return ADE_ROT_ANGLE_90_V_MIRROR;
        default:
            return rot;
    }
}

int getSclImageFormatType(int format)
{
    switch (format) {
       case ADE_RGB_565:
       case ADE_BGR_565:
       case ADE_XRGB_8888:
       case ADE_XBGR_8888:
       case ADE_ARGB_8888:
       case ADE_ABGR_8888:
       case ADE_RGBA_8888:
       case ADE_BGRA_8888:
       case ADE_RGB_888:
       case ADE_BGR_888:
            return ADE_SCL_ARGB888;
       case ADE_YUYV_I:
       case ADE_YVYU_I:
       case ADE_UYVY_I:
       case ADE_VYUY_I:
            return ADE_SCL_YUV422;
       case ADE_YUV444:
            return ADE_SCL_YUV444;
       case ADE_NV12:
       case ADE_NV21:
            return ADE_SCL_YUV420;
       default:
            return ADE_SCL_ARGB888;
    }
}

static int getScloFormatType(int format, int after_comp)
{
    switch (format) {
        case ADE_SCL_ARGB888:
            return ADE_SCL_ARGB888;
        case ADE_SCL_YUV422:
        case ADE_SCL_YUV444:
        case ADE_SCL_YUV420:
            if (after_comp == ADE_FALSE) {
                return ADE_SCL_YUV444;
            } else {
                return ADE_SCL_YUV422;
            }
        default:
            return ADE_SCL_ARGB888;
    }
}
int ade_cmdq_get_cmdfile_mem(struct ion_client *ade_ion_client, struct  cmdfile_buffer  *cf)
{
    u32     heap_mask = 0;
    u32     heap_flag = 0;
    struct iommu_map_format iommu_format;
    
    BUG_ON(cf == NULL);

    if (ade_ion_client == NULL) {
        balongfb_loge("ade_ion_client is null \n");
        return -EINVAL;
    }

    /* alloc un-cache memory for cmdfile,
     * if heap_flag = ION_FLAG_CACHED | ION_FLAG_CACHED_NEEDS_SYNC;
     * memory will be cache.
     */
    heap_mask = ION_HEAP(ION_SYSTEM_HEAP_ID);
    heap_flag = 0;

    /* get cmd buffer handle,  */
    cf->cmd_ion_handle = ion_alloc(ade_ion_client, cf->buff_size, IOMMU_PAGE_SIZE, heap_mask, heap_flag);
    if (IS_ERR(cf->cmd_ion_handle)) {
        balongfb_loge("ion_alloc alloc handle error \n");
        cf->cmd_ion_handle = NULL;
        return -EINVAL;
    }

    iommu_format.is_tile = 0;
    /* get cmd buffer phy addr, io addr for IP */
    if( ion_map_iommu(ade_ion_client, cf->cmd_ion_handle, &iommu_format) ){
        balongfb_loge("ion_alloc alloc handle error \n");
        cf->paddr = 0xdeadbeaf;
        return -EINVAL;
    }
    cf->paddr = iommu_format.iova_start;

    /* get cmf buffer virturl addr */
    cf->vaddr = ion_map_kernel(ade_ion_client, cf->cmd_ion_handle);

    if (IS_ERR(cf->vaddr)) {
        balongfb_loge("ion_alloc alloc handle error \n");
        return -EINVAL;
    }

    balongfb_logi(" cf->paddr = 0x%x \n", cf->paddr);
    return 0;
}

int ade_cmdq_cmdfile_buff_init(struct ion_client *ade_ion_client, struct  cmdqueue_buff   *cmdq_list)
{
    struct  cmdfile_buffer  *cf = NULL;
    struct  cmdqueue_buff   *cq = NULL;
    int i = 0;
    int j = 0;

    BUG_ON(cmdq_list == NULL);

    for (i = 0; i < ADE_CMDQ_TYPE_MAX; i++) {
        cq = &(cmdq_list[i]);
        cq->current_idx = 0;

        for (j = 0; j < ADE_CMD_FILE_BUFF_MAX; j++) {
            cf = &(cq->cf_list[j]);

            cf->buff_size = ADE_CMD_BUFF_SIZE_MAX;  /* 1M */
            cf->cmd_len   = 0;
            cf->is_busy   = ADE_FALSE;
            cf->next      = NULL;

            if(ade_cmdq_get_cmdfile_mem(ade_ion_client, cf) != 0) {
                balongfb_loge("ade_cmdq_get_cmdfile_mem get cmd buffer error \n");
                return -EINVAL;
            }
        }
    }

    wr_cmd.cmd_head.bits.cmd_type = ADE_CMD_TYPE_WRITE;
    wr_cmd.cmd_head.bits.arg_len  = 0;
    wr_cmd.cmd_head.bits.first_reg_offset = 0;

    return 0;
}

void ade_cmdq_wr_cmd2buff(void* cmdbuff_vaddr, u32 *cmd_len)
{
    u32   wr_cmd_len = 0;
#if ADE_DEBUG_LOG_ENABLE
    u32   str_len = 0;
    u32   i = 0;
#endif

    BUG_ON(cmd_len == NULL);

    if (wr_cmd.cmd_head.bits.first_reg_offset == ADE_INVAL_REG) {
        balongfb_logi_display_debugfs("ade_cmdq_wr_cmd2buff: first_reg_offset is inval \n");
        return;
    }

    wr_cmd_len = sizeof(wr_cmd.cmd_head.ul32) * (wr_cmd.cmd_head.bits.arg_len + 1);
    wr_cmd.cmd_head.bits.arg_len -= 1;

#if ADE_DEBUG_LOG_ENABLE
    if (g_command_file_wr_enable && (debug_cmd_vaddr != 0)) {
        memset(&str_cmd_file, 0, sizeof(str_cmd_file));
        /*snprintf(str_cmd_file, sizeof(str_cmd_file), "//cmd_type:%d arg_len:%d reg_offset:0x%x\n",
                    wr_cmd.cmd_head.bits.cmd_type,
                    wr_cmd.cmd_head.bits.arg_len + 1,
                    wr_cmd.cmd_head.bits.first_reg_offset);
        str_len = strlen(str_cmd_file); */
        snprintf(&(str_cmd_file[str_len]), sizeof(str_cmd_file) - str_len, "%08x\n", wr_cmd.cmd_head.ul32);
        str_len = strlen(str_cmd_file);

        for (i = 0; i < (wr_cmd.cmd_head.bits.arg_len + 1); i++) {
            snprintf(&(str_cmd_file[str_len]), sizeof(str_cmd_file) - str_len, "%08x\n", wr_cmd.reg_val[i]);
            str_len = strlen(str_cmd_file);
        }

        str_len = strlen(str_cmd_file);

        memcpy((char*)debug_cmd_vaddr + debug_str_len, &str_cmd_file, str_len);
        debug_str_len += str_len;
    }
#endif
    memcpy((char*)cmdbuff_vaddr + *cmd_len, &wr_cmd, wr_cmd_len);
    *cmd_len += wr_cmd_len;

    wr_cmd.cmd_head.bits.cmd_type = ADE_CMD_TYPE_WRITE;
    wr_cmd.cmd_head.bits.arg_len  = 0;
    wr_cmd.cmd_head.bits.first_reg_offset = ADE_INVAL_REG;
}

/************************* RDMA *********************************/
inline void ade_cmdq_wr_cmd(u32 reg_addr, u32 val)
{
    u32 last_reg;

    if (reg_addr == ADE_INVAL_REG) {
        return;
    }

    last_reg = wr_cmd.cmd_head.bits.arg_len * 4 + wr_cmd.cmd_head.bits.first_reg_offset;

    wr_cmd.cmd_head.bits.cmd_type = ADE_CMD_TYPE_WRITE;
    if ((last_reg == reg_addr) && (wr_cmd.cmd_head.bits.arg_len < ADE_CMD_WITE_REG_MAX)) {
        wr_cmd.reg_val[wr_cmd.cmd_head.bits.arg_len] = val;
        wr_cmd.cmd_head.bits.arg_len++;
    } else {
        wr_cmd.cmd_head.bits.first_reg_offset = reg_addr;
        wr_cmd.cmd_head.bits.arg_len = 1;
        wr_cmd.reg_val[0] = val;
    }
}

void ade_cmdq_wr_rdma_pe_cmd(u32 reg_addr, u32 ch_type, u32 rotation)
{
    volatile U_RD_CH1_PE rdma_pe;

    rdma_pe.u32 = 0;

    if (OVERLAY_PIPE_TYPE_ONLINE == ch_type) {
        rdma_pe.bits.rd_ch1_qos = 4;

        /* the min burst len is 16 when no rotation, is 4 when have online rotation */
        rdma_pe.bits.rd_ch1_min_burst_len = 0xf;
        if (rotation != ADE_ROT_NOP) {
            rdma_pe.bits.rd_ch1_min_burst_len = 0x3;
        }
    } else {
        rdma_pe.bits.rd_ch1_qos = 2;

        /* the min burst len is 8 when no rotation, is 4 when have offline rotation */
        rdma_pe.bits.rd_ch1_min_burst_len = 0x7;
        if (rotation != ADE_ROT_NOP) {
            rdma_pe.bits.rd_ch1_min_burst_len = 0x3;
        }
    }

    if (SFT_BOARD == fb_get_board_type()) {
        rdma_pe.bits.rd_ch1_min_burst_len = 0x3;
    }

    ade_cmdq_wr_cmd(reg_addr, rdma_pe.u32);
}

void ade_cmdq_wr_rdma_ctrl_cmd(u32 reg_addr, struct overlay_region_info *pipe_info)
{
    volatile U_RD_CH1_CTRL   rdma_ctrl;

    rdma_ctrl.u32 = 0;
    rdma_ctrl.bits.rd_ch1_axi_fsh_int_disable = 0;
    rdma_ctrl.bits.rd_ch1_fsh_int_disable     = 0;
    rdma_ctrl.bits.rd_ch1_format  = pipe_info->format;
    rdma_ctrl.bits.rd_ch1_rot     = pipe_info->rotation;

    ade_cmdq_wr_cmd(reg_addr, rdma_ctrl.u32);
}

void ade_cmdq_wr_rdma_ctrl_rot_cmd(u32 reg_addr, u32 format, ADE_ROT_UINT32  rotation)
{
    volatile U_RD_CH1_CTRL   rdma_ctrl;

    rdma_ctrl.u32 = 0;
    if (ADE_ROT_NOP == rotation) {
        rdma_ctrl.bits.rd_ch1_rd_dir  = 0;
        rdma_ctrl.bits.rd_ch1_partial = 0;
    } else if (ADE_ROT_90 == rotation) {
        rdma_ctrl.bits.rd_ch1_rd_dir  = 1;
        rdma_ctrl.bits.rd_ch1_partial = 2;
    } else if (ADE_ROT_180 == rotation) {
        rdma_ctrl.bits.rd_ch1_rd_dir  = 3;
        rdma_ctrl.bits.rd_ch1_partial = 1;
    } else if (ADE_ROT_270 == rotation) {
        rdma_ctrl.bits.rd_ch1_rd_dir  = 2;
        rdma_ctrl.bits.rd_ch1_partial = 2;
    } else if (ADE_ROT_V_MIRROR == rotation) {
        rdma_ctrl.bits.rd_ch1_rd_dir  = 1;
        rdma_ctrl.bits.rd_ch1_partial = 1;
    } else if (ADE_ROT_H_MIRROR == rotation) {
        rdma_ctrl.bits.rd_ch1_rd_dir  = 2;
        rdma_ctrl.bits.rd_ch1_partial = 1;
    } else if (ADE_ROT_90_V_MIRROR== rotation) {
        rdma_ctrl.bits.rd_ch1_rd_dir  = 3;
        rdma_ctrl.bits.rd_ch1_partial = 2;
    } else if (ADE_ROT_90_H_MIRROR == rotation) {
        rdma_ctrl.bits.rd_ch1_rd_dir  = 0;
        rdma_ctrl.bits.rd_ch1_partial = 2;
    }

    rdma_ctrl.bits.rd_ch1_axi_fsh_int_disable = 0;
    rdma_ctrl.bits.rd_ch1_fsh_int_disable     = 0;
    rdma_ctrl.bits.rd_ch1_format  = format;
    rdma_ctrl.bits.rd_ch1_rot     = rotation;

    ade_cmdq_wr_cmd(reg_addr, rdma_ctrl.u32);
}

/************************* WDMA *********************************/
void ade_cmdq_wr_wdma_ctrl_cmd(u32 reg_addr, struct overlay_compose_info *pcomp_info)
{
    volatile U_WR_CH1_CTRL  wr_ch_ctrl;

    BUG_ON(pcomp_info == NULL);

    wr_ch_ctrl.u32 = 0;
    wr_ch_ctrl.bits.wr_ch1_partial = 0;
    wr_ch_ctrl.bits.wr_ch1_wr_dir  = 0;
    wr_ch_ctrl.bits.wr_ch1_fsh_int_disable  = 0;
    wr_ch_ctrl.bits.wr_ch1_format  = pcomp_info->offline_dst_format;

    ade_cmdq_wr_cmd(reg_addr, wr_ch_ctrl.u32);
}

void ade_cmdq_wr_wdma3_ctrl_cmd(u32 reg_addr, struct overlay_compose_info *pcomp_info, u32 has_scl3)
{
    volatile U_WR_CH3_CTRL  wr_ch_ctrl;

    BUG_ON(pcomp_info == NULL);

    wr_ch_ctrl.u32 = 0;
    wr_ch_ctrl.bits.wr_ch3_partial = 0;
    wr_ch_ctrl.bits.wr_ch3_wr_dir  = 0;
    wr_ch_ctrl.bits.wr_ch3_fsh_int_disable  = 0;
    wr_ch_ctrl.bits.wr_ch3_format  = pcomp_info->wifi_dst_format;
    wr_ch_ctrl.bits.wr_ch3_yuv444_trans = 0;

    ade_cmdq_wr_cmd(reg_addr, wr_ch_ctrl.u32);
}

void ade_cmdq_wr_blk_rot_wdma3_ctrl_cmd(u32 reg_addr, u32 format)
{
    volatile U_WR_CH3_CTRL  wr_ch_ctrl;

    wr_ch_ctrl.u32 = 0;
    wr_ch_ctrl.bits.wr_ch3_partial = 1; /* first h, then v */
    wr_ch_ctrl.bits.wr_ch3_wr_dir  = 0;
    wr_ch_ctrl.bits.wr_ch3_fsh_int_disable  = 0;
    wr_ch_ctrl.bits.wr_ch3_format  = format;

    if ((format == ADE_YUYV_I) || (format == ADE_YVYU_I)
     || (format == ADE_UYVY_I) || (format == ADE_VYUY_I)) {
        wr_ch_ctrl.bits.wr_ch3_yuv444_trans = 1;
    }

    ade_cmdq_wr_cmd(reg_addr, wr_ch_ctrl.u32);
}

void ade_cmdq_wr_blk_rot_wdma1_ctrl_cmd(u32 reg_addr, u32 format)
{
    volatile U_WR_CH1_CTRL  wr_ch_ctrl;

    wr_ch_ctrl.u32 = 0;
    wr_ch_ctrl.bits.wr_ch1_partial = 1; /* first h, then v */
    wr_ch_ctrl.bits.wr_ch1_wr_dir  = 0;
    wr_ch_ctrl.bits.wr_ch1_fsh_int_disable  = 0;
    wr_ch_ctrl.bits.wr_ch1_format  = format;

    ade_cmdq_wr_cmd(reg_addr, wr_ch_ctrl.u32);
}




/************************* scl1,3 *********************************/
void ade_cmdq_wr_scl_ctrl_cmd(u32 reg_addr, struct overlay_region_info *pipe_info, struct ade_rect *src_rect, struct ade_rect *dst_rect)
{
    volatile U_ADE_SCL1_CTRL     scl1_ctrl;
    int v_ratio = 0;

    BUG_ON(pipe_info == NULL);

    scl1_ctrl.u32 = 0;
    scl_flag      = 0;

    scl1_ctrl.bits.form_i = getSclImageFormatType(pipe_info->format);
    scl1_ctrl.bits.form_o = getScloFormatType(scl1_ctrl.bits.form_i, ADE_FALSE);
    scl1_ctrl.bits.hfir_order = 0;
    scl1_ctrl.bits.a_scl_en   = 1;
    scl1_ctrl.bits.scl_en     = 1;
    scl1_ctrl.bits.vfir_en    = 1;
    scl1_ctrl.bits.hfir_en    = 1;
    scl1_ctrl.bits.v_tap      = 0;
    scl1_ctrl.bits.bypass     = 0;

    v_ratio = 10 * dst_rect->h / src_rect->h;
    if (v_ratio >= 4) {
        /* 4 vstap scl down 0.4 */
        scl1_ctrl.bits.v_tap = 0;
    } else {
        /* 6 vstap */
        scl1_ctrl.bits.v_tap = 1;

        /* v8r1, input rect.h is odd number, must be v_tap = 0;
         * v_tap must be 0 if input format is NV12/NV21 and input rect.h mod 4 isn't 0; */
        if ((HI6210_CHIPSET == get_chipset_type()) && ((src_rect->h % 2)
                             || ((scl1_ctrl.bits.form_i == ADE_SCL_YUV420) && (src_rect->h % 4) != 0)))
        {
            scl1_ctrl.bits.v_tap = 0;
        }

        if ((HI6220_CHIPSET == get_chipset_type()) && ((src_rect->h % 2)
                             || ((scl1_ctrl.bits.form_i == ADE_SCL_YUV420) && (src_rect->h % 4) != 0)))
        {
            scl_flag = 1;
        }
    }

    switch (scl1_ctrl.bits.v_tap) {
        case 0:
        {
            if ((ADE_SCL3_CTRL_REG == reg_addr) || (ADE_SCL1_CTRL_REG == reg_addr)) {
                if (src_rect->w > 1920) {
                    scl1_ctrl.bits.hfir_order = 0;
                }

                if (dst_rect->w > 1920) {
                    scl1_ctrl.bits.hfir_order = 1;
                }
            } else {
                /* scl2 ctrl */
                if (src_rect->w > 1280) {
                    scl1_ctrl.bits.hfir_order = 0;
                }

                if (dst_rect->w > 1280) {
                    scl1_ctrl.bits.hfir_order = 1;
                }
            }
        }
        break;
        case 1:
        {
            if ((ADE_SCL3_CTRL_REG == reg_addr) || (ADE_SCL1_CTRL_REG == reg_addr)) {
                if (src_rect->w > 960) {
                    scl1_ctrl.bits.hfir_order = 0;
                }

                if (dst_rect->w > 960) {
                    scl1_ctrl.bits.hfir_order = 1;
                }
            } else {
                /* scl2 ctrl */
                if (src_rect->w > 640) {
                    scl1_ctrl.bits.hfir_order = 0;
                }

                if (dst_rect->w > 640) {
                    scl1_ctrl.bits.hfir_order = 1;
                }
            }
        }
        break;
        default:
            break;
    }

    balongfb_logi_display_debugfs("ade_cmdq_wr_scl_ctrl_cmd: reg_addr = 0x%x, scl_ctrl = 0x%x \n", reg_addr, scl1_ctrl.u32);

    ade_cmdq_wr_cmd(reg_addr, scl1_ctrl.u32);
}
/******* scl2 ***************************/
void ade_cmdq_wr_scl2_ctrl_cmd(u32 reg_addr, struct ade_overlay_scl2 *scl2_info, struct ade_rect *src_rect, struct ade_rect *dst_rect)
{
    volatile U_ADE_SCL2_CTRL     scl2_ctrl;
    int v_ratio = 0;

    BUG_ON(scl2_info == NULL);
    scl_flag      = 0;
    scl2_ctrl.u32 = 0;
    scl2_ctrl.bits.form_i = ADE_SCL_ARGB888;
    scl2_ctrl.bits.form_o = ADE_SCL_ARGB888;
    scl2_ctrl.bits.hfir_order = 0;
    scl2_ctrl.bits.a_scl_en   = 1;
    scl2_ctrl.bits.scl_en     = 1;
    scl2_ctrl.bits.vfir_en    = 1;
    scl2_ctrl.bits.hfir_en    = 1;
    scl2_ctrl.bits.v_tap      = 0;
    scl2_ctrl.bits.bypass     = 0;

    v_ratio = 10 * dst_rect->h / src_rect->h;
    if (v_ratio >= 4) {
        /* 4 vstap */
        scl2_ctrl.bits.v_tap = 0;

        /* scl2 ctrl */
        if (src_rect->w > 1280) {
            scl2_ctrl.bits.hfir_order = 0;
        }

        if (dst_rect->w > 1280) {
            scl2_ctrl.bits.hfir_order = 1;
        }
    } else {
        /* 6 vstap */
        scl2_ctrl.bits.v_tap = 1;

        /* v8r1, input rect.h is odd number, must be v_tap = 0 */
        if ((HI6210_CHIPSET == get_chipset_type()) && (src_rect->h % 2))
        {
            scl2_ctrl.bits.v_tap = 0;
        }

        if ((HI6220_CHIPSET == get_chipset_type()) && (src_rect->h % 2))
        {
            scl_flag = 1;
        }

        if (src_rect->w > 640) {
            scl2_ctrl.bits.hfir_order = 0;
        }

        if (dst_rect->w > 640) {
            scl2_ctrl.bits.hfir_order = 1;
        }
    }

    balongfb_logi_display_debugfs("ade_cmdq_wr_scl2_ctrl_cmd: scl2_ctrl = 0x%x \n", scl2_ctrl.u32);

    ade_cmdq_wr_cmd(reg_addr, scl2_ctrl.u32);
}

/******* ovly scl ***************************/
void ade_cmdq_wr_ovly_scl_ctrl_cmd(u32 reg_addr, struct overlay_compose_info *pcomp_info)
{
    volatile U_ADE_SCL3_CTRL     scl3_ctrl;

    BUG_ON(pcomp_info == NULL);

    scl3_ctrl.u32 = 0;

    /* output format */
    scl3_ctrl.bits.form_o = getSclImageFormatType(pcomp_info->wifi_dst_format);
    scl3_ctrl.bits.form_i = (scl3_ctrl.bits.form_o == ADE_SCL_ARGB888) ? ADE_SCL_ARGB888 : ADE_SCL_YUV444;

    scl3_ctrl.bits.hfir_order = 0;
    scl3_ctrl.bits.a_scl_en   = 1;
    scl3_ctrl.bits.scl_en     = 1;
    scl3_ctrl.bits.vfir_en    = 1;
    scl3_ctrl.bits.hfir_en    = 1;
    scl3_ctrl.bits.v_tap      = 0;
    scl3_ctrl.bits.bypass     = 0;

    balongfb_logi_display_debugfs("ade_cmdq_wr_ovly_scl_ctrl_cmd: scl3_ctrl = 0x%x \n", scl3_ctrl.u32);

    ade_cmdq_wr_cmd(reg_addr, scl3_ctrl.u32);
}

/******* block rot scl: convert YUV420 to YUV444 ***************************/
void ade_cmdq_wr_blk_rot_scl_ctrl_cmd(u32 reg_addr, u32 input_format)
{
    volatile U_ADE_SCL1_CTRL     scl1_ctrl;

    if (input_format != ADE_SCL_YUV420) {
        balongfb_logi_display_debugfs("ade_cmdq_wr_blk_rot_scl_ctrl_cmd: input_format(%d) is invalid\n", input_format);
        return;
    }

    scl1_ctrl.u32 = 0;

    /* output format */
    scl1_ctrl.bits.form_i = input_format;
    scl1_ctrl.bits.form_o = ADE_SCL_YUV444;

    scl1_ctrl.bits.hfir_order = 0;
    scl1_ctrl.bits.a_scl_en   = 1;
    scl1_ctrl.bits.scl_en     = 0;
    scl1_ctrl.bits.vfir_en    = 1;
    scl1_ctrl.bits.hfir_en    = 1;
    scl1_ctrl.bits.v_tap      = 0;
    scl1_ctrl.bits.bypass     = 0;

    balongfb_logi_display_debugfs("ade_cmdq_wr_blk_rot_scl_ctrl_cmd: scl1_ctrl = 0x%x \n", scl1_ctrl.u32);

    ade_cmdq_wr_cmd(reg_addr, scl1_ctrl.u32);
}

void ade_cmdq_wr_scl_hsp_cmd(u32 reg_addr, struct ade_rect *src_rect, struct ade_rect *dst_rect)
{
    volatile U_ADE_SCL1_HSP scl1_hsp;
    s32 tmp;

    BUG_ON((src_rect == NULL) || (dst_rect == NULL));

    if (0 == dst_rect->w) {
        balongfb_loge("dst_rect->w is 0 \n");
        return;
    }

    scl1_hsp.u32 = 0;
    scl1_hsp.bits.h_step = (256 * 128 * src_rect->w) / dst_rect->w;

    tmp = (src_rect->w * 64 / dst_rect->w - 64);
    scl1_hsp.bits.h_offset = min(tmp, 127);

    balongfb_logi_display_debugfs("ade_cmdq_wr_scl_hsp_cmd: reg_addr = 0x%x,scl_hsp = 0x%x,v_offset=0x%x \n", reg_addr, scl1_hsp.u32, scl1_hsp.bits.h_offset);

    ade_cmdq_wr_cmd(reg_addr, scl1_hsp.u32);
}
void ade_cmdq_wr_scl_vsp_cmd(u32 reg_addr, struct ade_rect *src_rect, struct ade_rect *dst_rect)
{
    U_ADE_SCL1_VSP scl1_vsp;
    s32 tmp;

    BUG_ON((src_rect == NULL) || (dst_rect == NULL));

    if (0 == dst_rect->h) {
        balongfb_loge("dst_rect->h is 0 \n");
        return;
    }
    scl1_vsp.u32 = 0;
    scl1_vsp.bits.v_step = (256 * 128 * src_rect->h) / dst_rect->h;
    /* Modified for v8r2 chipset bugfix */
    if (scl_flag == 1 ) {
        scl1_vsp.bits.v_step = (256 * 128 * src_rect->h) / (dst_rect->h + 1);
        tmp = (src_rect->h * 64 / (dst_rect->h + 1) - 64);
    } else {
        tmp = (src_rect->h * 64 / dst_rect->h - 64);
    }
    scl1_vsp.bits.v_offset = min(tmp, 127);
    balongfb_logi_display_debugfs("ade_cmdq_wr_scl_vsp_cmd: reg_addr = 0x%x, scl_vsp = 0x%x, v_offset=0x%x\n",reg_addr, scl1_vsp.u32, scl1_vsp.bits.v_offset);

    ade_cmdq_wr_cmd(reg_addr, scl1_vsp.u32);
}

void ade_cmdq_wr_scl_hcoef_cmd(void *cmdbuff_vaddr, u32 *cmd_len, u32 reg_addr, struct ade_rect *src_rect, struct ade_rect *dst_rect)
{
    int i = 0;
    int j = 0;
    s32 const *hsp_coef;
    int h_ratio = 0;
    int coef_len;
    int tmp_cmd_len = 0;
    u32 tmp_reg;
    volatile U_ADE_SCL1_HCOEF scl_hcoef;

    BUG_ON((src_rect == NULL) || (dst_rect == NULL) || (cmd_len == NULL));

    if (src_rect->w == 0) {
        balongfb_loge("src_rect->w = 0 is error \n");
        return;
    }

    if (reg_addr == ADE_SCL3_HCOEF_0_REG) {
        j = 2;
    } else if (reg_addr == ADE_SCL2_HCOEF_0_REG) {
        j = 1;
    } else {
        j = 0;
    }

    h_ratio = 10 * dst_rect->w / src_rect->w;

    balongfb_logi_display_debugfs("ade_cmdq_wr_scl_hcoef_cmd: hratio = %d \n", h_ratio);
    balongfb_logi_display_debugfs("ade_cmdq_wr_scl_hcoef_cmd: scl_coef[%d].ul32 = 0x%x \n", j, scl_coef[j].ul32);

    if (h_ratio > 4) {
        hsp_coef = scl_lanczos2;
        coef_len = sizeof(scl_lanczos2) / sizeof(s32);
        if (scl_coef[j].bits.bit_h_lzs2) {
            balongfb_logi_display_debugfs("ade_cmdq_wr_scl_hcoef_cmd: scl_coef[%d].bit_h_lzs3 = 1 \n", j);
            return;
        }

        scl_coef[j].bits.bit_h_mit = 0;
        scl_coef[j].bits.bit_h_lzs2 = 1;
        scl_coef[j].bits.bit_h_lzs2_gaus10 = 0;
        scl_coef[j].bits.bit_h_lzs2_gaus40 = 0;

        scl_coef[j].bits.bit_h_lzs3 = 0;

    } else if (h_ratio <= 4 && h_ratio >= 3) {
        hsp_coef = scl_lanczos2_gaussian10;
        coef_len = sizeof(scl_lanczos2_gaussian10) / sizeof(s32);
        if (scl_coef[j].bits.bit_h_lzs2_gaus10) {
            balongfb_logi_display_debugfs("ade_cmdq_wr_scl_hcoef_cmd: scl_coef[%d].bit_h_lzs2_gaus10 = 1 \n", j);
            return;
        }

        scl_coef[j].bits.bit_h_lzs2 = 0;
        scl_coef[j].bits.bit_h_lzs3 = 0;
        scl_coef[j].bits.bit_h_mit = 0;
        scl_coef[j].bits.bit_h_lzs2_gaus40 = 0;
        scl_coef[j].bits.bit_h_lzs2_gaus10 = 1;
    } else {
        hsp_coef = scl_lanczos2_gaussian40;
        coef_len = sizeof(scl_lanczos2_gaussian40) / sizeof(s32);
        if (scl_coef[j].bits.bit_h_lzs2_gaus40) {
            balongfb_logi_display_debugfs("ade_cmdq_wr_scl_hcoef_cmd: scl_coef[%d].bit_h_lzs2_gaus40 = 1 \n", j);
            return;
        }

        scl_coef[j].bits.bit_h_lzs2 = 0;
        scl_coef[j].bits.bit_h_lzs3 = 0;
        scl_coef[j].bits.bit_h_mit = 0;
        scl_coef[j].bits.bit_h_lzs2_gaus10 = 0;
        scl_coef[j].bits.bit_h_lzs2_gaus40 = 1;
    }

    balongfb_logi_display_debugfs("ade_cmdq_wr_scl_hcoef_cmd: scl_coef[%d].ul32 = 0x%x \n", j, scl_coef[j].ul32);

    scl_hcoef.u32 = 0;
    tmp_reg = reg_addr;

#if 0
    while (tmp_cmd_len < coef_len) {
        scl_hcoef.bits.hcoef_m_2n = hsp_coef[tmp_cmd_len];
        scl_hcoef.bits.hcoef_m_2n_1 = hsp_coef[tmp_cmd_len + 1];
        outp32(balongfd_reg_base_ade + tmp_reg + i * 4, scl_hcoef.u32);
        tmp_cmd_len += 2;
        i++;
    }
#endif

    while (tmp_cmd_len < coef_len) {
        if (i < ADE_CMD_WITE_REG_MAX) {
            scl_hcoef.bits.hcoef_m_2n = hsp_coef[tmp_cmd_len];
            scl_hcoef.bits.hcoef_m_2n_1 = hsp_coef[tmp_cmd_len + 1];
            ade_cmdq_wr_cmd(tmp_reg + i * 4, scl_hcoef.u32);
            i++;
            tmp_cmd_len += 2;
        } else {
            tmp_reg += i * 4;
            ade_cmdq_wr_cmd2buff(cmdbuff_vaddr, cmd_len);
            i = 0;
        }
    }

    ade_cmdq_wr_cmd2buff(cmdbuff_vaddr, cmd_len);

}
void ade_cmdq_wr_scl_vcoef_cmd(void *cmdbuff_vaddr, u32 *cmd_len, u32 reg_addr, struct ade_rect *src_rect, struct ade_rect *dst_rect)
{
    int i = 0;
    int j = 0;
    s32 const *vsp_coef = NULL;
    int v_ratio = 0;
    int coef_len;
    int tmp_cmd_len = 0;
    u32 tmp_reg;
    volatile U_ADE_SCL1_VCOEF scl_vcoef;

    BUG_ON((src_rect == NULL) || (dst_rect == NULL) || (cmd_len == NULL));

    if (src_rect->h == 0) {
        balongfb_loge("src_rect->h = 0 is error \n");
        return;
    }

    if (reg_addr == ADE_SCL3_VCOEF_0_REG) {
        j = 2;
    } else if (reg_addr == ADE_SCL2_VCOEF_0_REG) {
        j = 1;
    } else {
        j = 0;
    }

    v_ratio = 10 * dst_rect->h / src_rect->h;

    vsp_coef = scl_lanczos2;
    coef_len = sizeof(scl_lanczos2) / sizeof(s32);

    balongfb_logi_display_debugfs("ade_cmdq_wr_scl_vcoef_cmd: v_ratio = %d \n", v_ratio);
    balongfb_logi_display_debugfs("ade_cmdq_wr_scl_vcoef_cmd: scl_coef[%d].ul32 = 0x%x \n", j, scl_coef[j].ul32);
    if (v_ratio > 3) {
        vsp_coef = scl_lanczos2;
        coef_len = sizeof(scl_lanczos2) / sizeof(s32);
        if (scl_coef[j].bits.bit_v_lzs2) {
            balongfb_logi_display_debugfs("ade_cmdq_wr_scl_vcoef_cmd: scl_coef[%d].bits.bit_v_lzs3 = 1 \n", j);
            return;
        }

        scl_coef[j].bits.bit_v_mit = 0;
        scl_coef[j].bits.bit_v_lzs2 = 1;
        scl_coef[j].bits.bit_v_lzs2_gaus10 = 0;
        scl_coef[j].bits.bit_v_lzs2_gaus40 = 0;

        scl_coef[j].bits.bit_v_lzs3 = 0;

    } else if (v_ratio == 3) {
        vsp_coef = scl_lanczos2_gaussian10;
        coef_len = sizeof(scl_lanczos2_gaussian10) / sizeof(s32);
        if (scl_coef[j].bits.bit_v_lzs2_gaus10) {
            balongfb_logi_display_debugfs("ade_cmdq_wr_scl_vcoef_cmd: scl_coef[%d].bits.bit_v_lzs2_gaus10 = 1 \n", j);
            return;
        }

        scl_coef[j].bits.bit_v_lzs2 = 0;
        scl_coef[j].bits.bit_v_lzs3 = 0;
        scl_coef[j].bits.bit_v_mit = 0;
        scl_coef[j].bits.bit_v_lzs2_gaus40 = 0;
        scl_coef[j].bits.bit_v_lzs2_gaus10 = 1;
    } else {
        vsp_coef = scl_lanczos2_gaussian40;
        coef_len = sizeof(scl_lanczos2_gaussian40) / sizeof(s32);
        if (scl_coef[j].bits.bit_v_lzs2_gaus40) {
            balongfb_logi_display_debugfs("ade_cmdq_wr_scl_vcoef_cmd: scl_coef[%d].bits.bit_v_lzs2_gaus40 = 1 \n", j);
            return;
        }

        scl_coef[j].bits.bit_v_lzs2 = 0;
        scl_coef[j].bits.bit_v_lzs3 = 0;
        scl_coef[j].bits.bit_v_mit = 0;
        scl_coef[j].bits.bit_v_lzs2_gaus10 = 0;
        scl_coef[j].bits.bit_v_lzs2_gaus40 = 1;
    }

    balongfb_logi_display_debugfs("ade_cmdq_wr_scl_vcoef_cmd: scl_coef[%d].ul32 = 0x%x \n", j, scl_coef[j].ul32);

    scl_vcoef.u32 = 0;
    tmp_reg = reg_addr;
#if 0
    while (tmp_cmd_len < coef_len) {
        scl_vcoef.bits.vcoef_i_2j = vsp_coef[tmp_cmd_len];
        scl_vcoef.bits.vcoef_i_2j_1 = vsp_coef[tmp_cmd_len + 1];
        outp32(balongfd_reg_base_ade + tmp_reg + i * 4, scl_vcoef.u32);
        tmp_cmd_len += 2;
        i++;
    }
#endif

    while (tmp_cmd_len < coef_len) {
        if (i < ADE_CMD_WITE_REG_MAX) {
            scl_vcoef.bits.vcoef_i_2j = vsp_coef[tmp_cmd_len];
            scl_vcoef.bits.vcoef_i_2j_1 = vsp_coef[tmp_cmd_len + 1];
            ade_cmdq_wr_cmd(tmp_reg + i * 4, scl_vcoef.u32);
            i++;
            tmp_cmd_len += 2;
        } else {
            tmp_reg += i * 4;
            ade_cmdq_wr_cmd2buff(cmdbuff_vaddr, cmd_len);
            i = 0;
        }
    }

    ade_cmdq_wr_cmd2buff(cmdbuff_vaddr, cmd_len);

}


/************************* ctrans *********************************/
void ade_cmdq_wr_ctran_chdc_cmd_with_yuv_info(u32 reg_addr, u32 val, u32 yuv_info)
{
    u32  *dc = NULL;
    int   len = 0;
    int   i = 0;

     /* 0: csc,yuv2rgb, 1: fmc, rgb2yuv */
     /* 0:YUV444>ARGB8888; 1:ARGB8888>YUV422; 2:ARGB8888>YUV444; 3:ARGB8888>ARGB8888 */
    if (val == 3) {
        dc = dc_601_wide_rgb2rgb;
        len = sizeof(dc_601_wide_rgb2rgb) / sizeof(u32);
    } else if (val == 0) {
        dc  = getCtranYuv2rgbChdcArray(yuv_info, &len);
    } else {
        dc = dc_601_wide_rgb2yuv;
        len = sizeof(dc_601_wide_rgb2yuv) / sizeof(u32);
    }

    while (i < len) {
        ade_cmdq_wr_cmd(reg_addr + 2*i, dc[i + 1] << 16 | dc[i]);
        i += 2;
    }
}
void ade_cmdq_wr_ctran_csc_cmd_with_yuv_info(u32 reg_addr, u32 val, u32 yuv_info)
{
    u32  *csc = NULL;
    int   len = 0;
    int   i = 0;

    /* 0: csc,yuv2rgb, 1: fmc, rgb2yuv */
    /* 0:YUV444>ARGB8888; 1:ARGB8888>YUV422; 2:ARGB8888>YUV444; 3:ARGB8888>ARGB8888 */
    if (val == 3) {
        down(&g_ct_sem);
        csc = g_601_csc_value;
        len = sizeof(g_601_csc_value) / sizeof(u32);
        while (i < len) {
            ade_cmdq_wr_cmd(reg_addr + 2*i, csc[i+ 1] << 16 | csc[i]);
            i += 2;
        }
        up(&g_ct_sem);
        return;
    } else if (val == 0) {
        csc = getCtranYuv2rgbCscArray(yuv_info, &len);
    } else {
        csc = csc_601_wide_rgb2yuv;
        len = sizeof(csc_601_wide_rgb2yuv) / sizeof(u32);
    }

    while (i < len) {
        ade_cmdq_wr_cmd(reg_addr + 2*i, csc[i+ 1] << 16 | csc[i]);
        i += 2;
    }
}

void ade_cmdq_wr_ctran_chdc_cmd(u32 reg_addr, u32 val)
{
    ade_cmdq_wr_ctran_chdc_cmd_with_yuv_info(reg_addr, val, ADE_YUV_601_WIDE);
}

void ade_cmdq_wr_ctran_csc_cmd(u32 reg_addr, u32 val)
{
    ade_cmdq_wr_ctran_csc_cmd_with_yuv_info(reg_addr, val, ADE_YUV_601_WIDE);
}


/*
 * Alpha blending formula in different mode:
 * 1. When source's global alpha is valid and there is alpha channel,
 *    the formula is globalAlpha_src*RGB_src + (1 - globalAlpha_src*pixelAlpha_src)*RGB_dst
 *    in case of pre-multi blending mode.
 *
 * 2. When source's global alpha is valid and there is alpha channel,
 *    the formula is pixelAlpha_src*globalAlpha_src*RGB_src + (1 - globalAlpha_src*pixelAlpha_src)*RGB_dst
 *    in case of coverage blending mode.
 *
 * 3. When source's global alpha is valid and there is no alpha channel,
 *    the formula is globalAlpha_src*RGB_src + (1 - globalAlpha_src)*RGB_dst
 *    in case of pre-multi blending mode or coverage blending mode.
 *
 * 4. When source's global alpha is invalid and there is alpha channel,
 *    the formula is RGB_src + (1 - pixelAlpha_src)*RGB_dst
 *    in case of pre-multi blending mode.
 *
 * 5. When source's global alpha is invalid and there is alpha channel,
 *    the formula is pixelAlpha_src*RGB_src + (1 - pixelAlpha_src)*RGB_dst
 *    in case of coverage blending mode.
 */
void ade_cmdq_wr_ovly_ch_ctrl_cmd(u32 reg_addr, struct ovly_ctrl_info ctrl_info)
{
    u32     blending;
    u32     panelAlpha      = 0xff;
    bool    per_pixel_alpha = true;
    u32     alp_sel_coeff   = ADE_ALP_MUL_COEFF_3;
    u32     under_alp_coeff = ADE_ALP_MUL_COEFF_2;
    u32     ch_alp_mode     = ADE_ALP_GLOBAL;
    u32     dim_blending    = 0;
    u32     dim_alpha;
    volatile U_ADE_OVLY_CH1_CTL   ovly_ch_ctrl;

    ovly_ch_ctrl.u32 = 0;

    dim_blending = ctrl_info.dim_blending;
    dim_alpha    = dim_blending >> 16;

    balongfb_logi_display_debugfs("ade_cmdq_wr_ovly_ch_ctrl_cmd: ctrl_info.ch_num = %d, ctrl_info.ovly_type=%d \n", ctrl_info.ch_num, ctrl_info.ovly_type);

    if (ADE_OVLY_TYPE_BYPASS == ctrl_info.ovly_type) {
        /* bypass ovly */
        alp_sel_coeff = ADE_ALP_MUL_COEFF_3;    /* 1 */
        under_alp_coeff = ADE_ALP_MUL_COEFF_0;  /* alpha */

        /* per_pixel */
        dim_blending = 0;
        panelAlpha = 0xff;
        per_pixel_alpha = true;
    } else if (ADE_OVLY_TYPE_SEC_OVLY == ctrl_info.ovly_type) {
        /* sec ovly */
        alp_sel_coeff = ADE_ALP_MUL_COEFF_3;    /* 1 */
        under_alp_coeff = ADE_ALP_MUL_COEFF_0;  /* alpha */

        /* per_pixel */
        dim_blending = 0;
        panelAlpha = 0xff;
        per_pixel_alpha = true;
    } else {
        blending = ctrl_info.blending;
        per_pixel_alpha = isAlphaRGBType(ctrl_info.format);
        panelAlpha = blending >> 16;

        balongfb_logi_display_debugfs("ade_cmdq_wr_ovly_ch_ctrl_cmd, blending = 0x%x, per_pixel_alpha = %d, panelAlpha = %d \n",\
                                  blending, per_pixel_alpha, panelAlpha);


        /* first ovly */
        switch(blending & 0xffff) {
            case OVERLAY_BLENDING_PREMULT:
                alp_sel_coeff   = ADE_ALP_MUL_COEFF_3; /* 1 */
                under_alp_coeff = ADE_ALP_MUL_COEFF_1; /* 1 - alpha */
                break;
            case OVERLAY_BLENDING_COVERAGE:
                alp_sel_coeff   = ADE_ALP_MUL_COEFF_0; /* alpha */
                under_alp_coeff = ADE_ALP_MUL_COEFF_1; /* 1 - alpha */

                if (per_pixel_alpha) {
                    ch_alp_mode = ADE_ALP_PIXEL_AND_GLB;
                }
                break;
            case OVERLAY_BLENDING_SEC_OVLY:
                alp_sel_coeff   = ADE_ALP_MUL_COEFF_3; /* 1 */
                under_alp_coeff = ADE_ALP_MUL_COEFF_0; /* alpha */
                panelAlpha = 0xff;
                break;
            case OVERLAY_BLENDING_PIX_GLB:
                alp_sel_coeff   = ADE_ALP_MUL_COEFF_3; /* 1 */
                under_alp_coeff = ADE_ALP_MUL_COEFF_1; /* 1 - alpha */
                ch_alp_mode = ADE_ALP_PIXEL_AND_GLB;
                per_pixel_alpha = true;
                break;
            default:
                balongfb_logi_display_debugfs(" blending none 0x%x \n", blending);

                /* Disable alpha blending */
                alp_sel_coeff   = ADE_ALP_MUL_COEFF_3; /* 1 */
                under_alp_coeff = ADE_ALP_MUL_COEFF_2; /* 0 */
                panelAlpha = 0xff;
                per_pixel_alpha = false;
                break;
        }
    }

    if (panelAlpha < 0xff) {
        if (ch_alp_mode != ADE_ALP_PIXEL_AND_GLB) {
            alp_sel_coeff   = ADE_ALP_MUL_COEFF_0;            /* alpha */
        }

        ovly_ch_ctrl.bits.ch1_alp_mode = ch_alp_mode;
        ovly_ch_ctrl.bits.ch1_alp_gbl = panelAlpha;
        ovly_ch_ctrl.bits.ch1_alp_sel  = alp_sel_coeff;
        ovly_ch_ctrl.bits.ch1_under_alp_sel  = under_alp_coeff;
        ovly_ch_ctrl.bits.ch1_en  = ADE_ENABLE;
        ovly_ch_ctrl.bits.ch1_under_alp_gbl = 0;
    } else if (per_pixel_alpha) {
        ovly_ch_ctrl.bits.ch1_alp_gbl = panelAlpha;
        ovly_ch_ctrl.bits.ch1_alp_mode = ADE_ALP_PIXEL;
        ovly_ch_ctrl.bits.ch1_alp_sel  = alp_sel_coeff;
        ovly_ch_ctrl.bits.ch1_under_alp_sel  = under_alp_coeff;
        ovly_ch_ctrl.bits.ch1_en  = ADE_ENABLE;
        ovly_ch_ctrl.bits.ch1_under_alp_gbl = 0;
    } else {
        /* Disable alpha blending */
        ovly_ch_ctrl.bits.ch1_alp_gbl = panelAlpha;
        ovly_ch_ctrl.bits.ch1_alp_mode = ADE_ALP_PIXEL;
        ovly_ch_ctrl.bits.ch1_alp_sel  = ADE_ALP_MUL_COEFF_3;
        ovly_ch_ctrl.bits.ch1_under_alp_sel  = ADE_ALP_MUL_COEFF_2;
        ovly_ch_ctrl.bits.ch1_en  = ADE_ENABLE;
        ovly_ch_ctrl.bits.ch1_under_alp_gbl = 0;
    }

    /* dim layer */
    if ((dim_blending & 0xffff) ==  OVERLAY_BLENDING_DIM) {
        if (dim_alpha > 0xFF) {
            dim_alpha = 0xFF;
        }

        /* DIM alpha value is opposite between android and ADE. */
        dim_alpha = 0xFF - dim_alpha;

        ovly_ch_ctrl.bits.ch1_under_alp_gbl = dim_alpha;
        ovly_ch_ctrl.bits.ch1_dimming_en    = 1;
    }

    /* ch select */
    if ((ctrl_info.sec_ovly_ch_count > 0) && (ctrl_info.ovly_num != OVERLAY_OVLY_3)) {
        ovly_ch_ctrl.bits.ch1_sel = 0;      /* sec_Ovly */
    } else {
        ovly_ch_ctrl.bits.ch1_sel = ctrl_info.ch_num + 1;  /* ch */
    }

    balongfb_logi_display_debugfs("ade_cmdq_wr_ovly_ch_ctrl_cmd: ovly_ch_ctrl.u32 = 0x%x, ctrl_info.ch_num =%d \n", ovly_ch_ctrl.u32,  ctrl_info.ch_num );

    ade_cmdq_wr_cmd(reg_addr, ovly_ch_ctrl.u32);
}

/************************* rot *********************************/
void ade_cmdq_wr_rot_cfg_cmd(u32 reg_addr, struct ade_overlay_rot *rot_info)
{
    volatile U_ADE_ROT_CFG   rot_cfg;

    BUG_ON(rot_info == NULL);

    rot_cfg.u32 = 0;
    rot_cfg.bits.rot_angle = getRdmaRot2RotAngle(rot_info->rotation);

    balongfb_logi_display_debugfs("ade_cmdq_wr_rot_cfg_cmd: rotation:%d \n", rot_info->rotation);

    if ((rot_info->rot_buff == TOP_ROT_BUFF_ROT_1080p) && (rot_info->rot_width <= ADE_ROT_WIDTH_MAX)) {
        rot_cfg.bits.block_width = 15;
    } else {
        rot_cfg.bits.block_width = 7;
    }

    ade_cmdq_wr_cmd(reg_addr, rot_cfg.u32);
}

void ade_cmdq_wr_wait_cmd(void *cmdbuff_vaddr, u32 *cmd_len, u32 val)
{
    WAIT_CMD  wait_cmd;
#if ADE_DEBUG_LOG_ENABLE
    u32   str_len;
#endif

    BUG_ON(cmd_len == NULL);

    balongfb_logi_display_debugfs("ade_cmdq_wr_wait_cmd: enter succ, val= %d \n", val);

    wait_cmd.ul32 = 0;
    wait_cmd.bits.cmd_type = ADE_CMD_TYPE_WAIT;
    wait_cmd.bits.event_no = val;

    memcpy((char*)cmdbuff_vaddr + *cmd_len, &wait_cmd.ul32, sizeof(wait_cmd.ul32));
    *cmd_len += sizeof(wait_cmd.ul32);

    balongfb_logi_display_debugfs("cmdbuff_vaddr = 0x%p, *cmd_len = %d, wait_cmd.ul32=0x%x \n", cmdbuff_vaddr, *cmd_len, wait_cmd.ul32);

#if ADE_DEBUG_LOG_ENABLE
    if (g_command_file_wr_enable) {
        memset(&str_cmd_file, 0, sizeof(str_cmd_file));
        snprintf(str_cmd_file, sizeof(str_cmd_file), "//cmd_type:wait\n%08x\n", wait_cmd.ul32);

        str_len = strlen(str_cmd_file);
        memcpy((char*)debug_cmd_vaddr + debug_str_len, &str_cmd_file, str_len);
        debug_str_len += str_len;

        balongfb_logi_display_debugfs("ade_cmdq_wr_wait_cmd : str_cmd_file = %s \n", str_cmd_file);
    }
#endif
}

void ade_cmdq_wr_eof_cmd(void *cmdbuff_vaddr, u32 *cmd_len)
{
    NULL_EOF_CMD  eof_cmd;
    NULL_EOF_CMD  null_cmd;
    u32 align_count = 0;
    unsigned   i = 0;
#if ADE_DEBUG_LOG_ENABLE
    u32   str_len = 0;
    u32   debug_cmd_len = *cmd_len;
#endif

    balongfb_logi_display_debugfs("ade_cmdq_wr_eof_cmd:enter, cmdbuff_vaddr= 0x%p, *cmd_len = %d \n", cmdbuff_vaddr, *cmd_len);

    eof_cmd.ul32 = 0;
    eof_cmd.bits.cmd_type = ADE_CMD_TYPE_EOF;

    null_cmd.ul32 = 0;
    null_cmd.bits.cmd_type = ADE_CMD_TYPE_NULL;

    align_count = (*cmd_len + sizeof(eof_cmd.ul32)) % ADE_CMD_ALIGN_BYTE_LEN ;
    if (align_count != 0) {
        /* align up to 16*4 byte */
        align_count = (((*cmd_len / ADE_CMD_ALIGN_BYTE_LEN + 1) * ADE_CMD_ALIGN_BYTE_LEN) - sizeof(eof_cmd.ul32) - *cmd_len) / 4;
        for (i = 0; i < align_count; i++) {
            memcpy((char*)cmdbuff_vaddr + *cmd_len, &null_cmd, sizeof(null_cmd.ul32));
            *cmd_len += sizeof(null_cmd.ul32);
        }
    }

    memcpy((char*)cmdbuff_vaddr + *cmd_len, &eof_cmd.ul32, sizeof(eof_cmd.ul32));
    *cmd_len += sizeof(eof_cmd.ul32);

#if ADE_DEBUG_LOG_ENABLE
    if (g_command_file_wr_enable && (debug_cmd_vaddr != 0)) {
        memset(&str_cmd_file, 0, sizeof(str_cmd_file));

        /* align up to 16*4 byte */
        align_count = debug_cmd_len % (ADE_CMD_ALIGN_BYTE_LEN - sizeof(eof_cmd.ul32));
        if (align_count != 0) {
            align_count = (ADE_CMD_ALIGN_BYTE_LEN - sizeof(eof_cmd.ul32) - align_count) / 4;
            for (i = 0; i < align_count; i++) {
                snprintf(&str_cmd_file[str_len], sizeof(str_cmd_file) - str_len, "%08x\n", null_cmd.ul32);
                str_len = strlen(str_cmd_file);
            }
        }
        /*
        snprintf(str_cmd_file, sizeof(str_cmd_file), "//cmd_type:EOF\n%08x\n", eof_cmd.ul32);
        str_len = strlen(str_cmd_file);  */
        memcpy((char*)debug_cmd_vaddr + debug_str_len, &str_cmd_file, str_len);
        debug_str_len += str_len;

        balongfb_logi_display_debugfs("ade_cmdq_wr_eof_cmd : str_cmd_file = %s \n", str_cmd_file);
    }
#endif
}

#if ADE_DEBUG_LOG_ENABLE
int ade_cmdq_wr_cmdbuff2file(struct cmdfile_buffer  *cf_buff, bool is_online, u32 pattern_num)
{
    struct file* cmd_file = NULL;
    struct file* debug_cmd_file;
    mm_segment_t   fs;
    loff_t  pos = 0;
    loff_t  cmd_pos = 0;
    int len = 0;
    char  rm_cmd_file_name[64] = {0};
    char  debug_cmd_file_name[64] = {0};

    if (is_online) {
        snprintf(rm_cmd_file_name, sizeof(rm_cmd_file_name), "/data/ade_rm_online_cmd_file_%d.txt", pattern_num);
        snprintf(debug_cmd_file_name, sizeof(debug_cmd_file_name), "/data/ade_debug_online_cmd_file_%d.txt", pattern_num);
    } else {
        snprintf(rm_cmd_file_name, sizeof(rm_cmd_file_name), "/data/ade_rm_offline_cmd_file_%d.txt", pattern_num);
        snprintf(debug_cmd_file_name, sizeof(debug_cmd_file_name), "/data/ade_debug_offline_cmd_file_%d.txt", pattern_num);
    }

    balongfb_logi("enter \n");

    if (debug_cmd_vaddr != 0) {
        cmd_file = filp_open(rm_cmd_file_name, O_RDWR | O_CREAT, 0666);
        if (IS_ERR(cmd_file)) {
            balongfb_loge("create ade cmd_file error!");
            return -EINVAL;
        }
    }

    debug_cmd_file = filp_open(debug_cmd_file_name, O_RDWR | O_CREAT, 0666);
    if (IS_ERR(debug_cmd_file)) {
        balongfb_loge("create ade debug_cmd_file error!");
        return -EINVAL;
    }

    fs = get_fs();
    set_fs(KERNEL_DS);

    /* rm */
    if (cmd_file != NULL) {
        len = vfs_write(cmd_file, (void*)debug_cmd_vaddr, debug_str_len, &pos);
        debug_str_len = 0;

        if (len <= 0) {
            balongfb_loge("vfs_write error");
            filp_close(cmd_file, NULL);
            return -EINVAL;
        }
    }

    /* debug */
    len = vfs_write(debug_cmd_file, (void*)cf_buff->vaddr, cf_buff->cmd_len, &cmd_pos);

    if (len <= 0) {
        balongfb_loge("vfs_write error");
        filp_close(debug_cmd_file, NULL);
        return -EINVAL;
    }

    balongfb_logi("cf_buff->vaddr = %p, cmd_len = %d \n", cf_buff->vaddr,cf_buff->cmd_len);

    if (cmd_file != NULL) {
        filp_close(cmd_file, NULL);
    }

    filp_close(debug_cmd_file, NULL);
    set_fs(fs);

    file_count++;

    return 0;
}

int ade_cmdq_wr_topinfo2file(char* buff, int len)
{
    mm_segment_t   fs;
    loff_t  pos = 0;
    int     wr_len = 0;
    struct file* top_file;

    top_file = filp_open("/data/ade_top_file.txt", O_RDWR | O_CREAT | O_APPEND, 0666);
    if (IS_ERR(top_file)) {
        balongfb_loge("create ade top_file error!");
        return -EINVAL;
    }

    fs = get_fs();
    set_fs(KERNEL_DS);

    wr_len = vfs_write(top_file, buff, len, &pos);

    if (wr_len < 0) {
        filp_close(top_file, NULL);
        balongfb_loge("vfs_write error, wr_len = %d", wr_len);
        return -EINVAL;
    }

    filp_close(top_file, NULL);
    set_fs(fs);

    return 0;
}

#endif

