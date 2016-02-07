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

#ifndef __K3_HDMI_LOG_H__
#define __K3_HDMI_LOG_H__

#include<linux/kernel.h>

extern u8 g_msg_print_level;

#ifndef LOG_TAG
#define LOG_TAG "hdmi"
#endif

#define DEBUG_LEVEL 0

#define logd(fmt, ...) \
do {\
    if (g_msg_print_level > 7) {\
        pr_info(LOG_TAG"[D]:%s:%d: "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
    }\
} while(0);

#define logi(fmt, ...) \
do {\
    if (g_msg_print_level > 6) {\
        pr_notice(LOG_TAG"[I]:%s:%d: "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
    }\
} while(0);


#define logw(fmt, ...) \
do {\
    if (g_msg_print_level > 5) {\
        pr_warn(LOG_TAG"[W]:%s:%d: "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
    }\
} while(0);

#define loge(fmt, ...) \
do {\
    if (g_msg_print_level > 4) {\
        pr_err(LOG_TAG"[E]:%s:%d: "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
    }\
} while(0);

#define IN_FUNCTION   logd("in.\n");
#define OUT_FUNCTION  logd("out.\n");

#endif

