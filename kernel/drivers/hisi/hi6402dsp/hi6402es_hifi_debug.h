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
#ifndef __HI6402ES_DSP_DEBUG_H__
#define __HI6402ES_DSP_DEBUG_H__

extern unsigned long hi6402es_dsp_debug_level;

#define HI6402ES_DSP_ERROR(msg, ...)    \
    do {                          \
		if (hi6402es_dsp_debug_level > 0)  \
			printk(KERN_ERR "[hi6402es-dsp]%s: "msg, __func__, ## __VA_ARGS__); \
       } while (0)

#define HI6402ES_DSP_WARNING(msg, ...)    \
    do {                          \
		if (hi6402es_dsp_debug_level > 1)  \
			printk(KERN_WARNING "[hi6402es-dsp]%s: "msg, __func__, ## __VA_ARGS__); \
       } while (0)

#define HI6402ES_DSP_INFO(msg, ...)    \
    do {                          \
		if (hi6402es_dsp_debug_level > 2)  \
			printk(KERN_INFO "[hi6402es-dsp]%s: "msg, __func__, ## __VA_ARGS__); \
       } while (0)

#define HI6402ES_DSP_DEBUG(msg, ...)    \
    do {                          \
		if (hi6402es_dsp_debug_level > 3)  \
			printk(KERN_INFO "[hi6402es-dsp]%s: "msg, __func__, ## __VA_ARGS__); \
       } while (0)

#define IN_FUNCTION  HI6402ES_DSP_INFO("in. \n");
#define OUT_FUNCTION HI6402ES_DSP_INFO("out. \n");

#endif
