/* Copyright (c) 2013-2014, Hisilicon Tech. Co., Ltd. All rights reserved.
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
*/
#ifndef __HI6402_DSP_DEBUG_H__
#define __HI6402_DSP_DEBUG_H__

extern unsigned long hi6402_dsp_debug_level;

#define HI6402_DSP_ERROR(msg, ...)    \
    do {                          \
		if (hi6402_dsp_debug_level > 0)  \
			printk(KERN_ERR "[hi6402-dsp]%s: "msg, __func__, ## __VA_ARGS__); \
       } while (0)

#define HI6402_DSP_WARNING(msg, ...)    \
    do {                          \
		if (hi6402_dsp_debug_level > 1)  \
			printk(KERN_WARNING "[hi6402-dsp]%s: "msg, __func__, ## __VA_ARGS__); \
       } while (0)

#define HI6402_DSP_INFO(msg, ...)    \
    do {                          \
		if (hi6402_dsp_debug_level > 2)  \
			printk(KERN_INFO "[hi6402-dsp]%s: "msg, __func__, ## __VA_ARGS__); \
       } while (0)

#define HI6402_DSP_DEBUG(msg, ...)    \
    do {                          \
		if (hi6402_dsp_debug_level > 3)  \
			printk(KERN_INFO "[hi6402-dsp]%s: "msg, __func__, ## __VA_ARGS__); \
       } while (0)

#define IN_FUNCTION  HI6402_DSP_DEBUG("++\n");
#define OUT_FUNCTION HI6402_DSP_DEBUG("--\n");

#endif
