/* Copyright (c) 2008-2011, Hisilicon Tech. Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef __BALONG_FB_DEF_H__
#define __BALONG_FB_DEF_H__

#include <linux/delay.h>
#include <linux/string.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <asm/bug.h>

/* for debug test compile */
#ifdef CONFIG_DEBUG_FS
#define  ADE_DEBUG_LOG_ENABLE       (1)
#else
#define  ADE_DEBUG_LOG_ENABLE       (0)
#endif

//ADE fence
#define ADE_SYNC_SUPPORT             (1)

#define LCD_ESD_CHECK_SUPPORT	(1)

#define LCD_CHECK_MIPI_SUPPORT	(1)
#ifndef MAX
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#endif

#ifndef MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

/* align */
#ifndef ALIGN_DOWN
#define ALIGN_DOWN(val, al)  ((val) & ~((al)-1))
#endif
#ifndef ALIGN_UP
#define ALIGN_UP(val, al)    (((val) + ((al)-1)) & ~((al)-1))
#endif

#ifndef BIT
#define BIT(x)  (1<<(x))
#endif

/* x/y, and align up. eg. ROUND(9,5) is 2 */
#define   ROUND(x,y)   ((x) / (y) + ((x) % (y) *10 / (y) >= 5 ? 1 : 0))

/*--------------------------------------------------------------------------*/
#define NDEBUG 0

#if NDEBUG
#define balongfb_logd(fmt, ...)
#else
#define balongfb_logd(fmt, ...) \
	pr_debug("[balongfb]%s: "fmt, __func__, ##__VA_ARGS__)
#endif
#define balongfb_logi(fmt, ...) \
	pr_info("[balongfb]%s: "fmt, __func__, ##__VA_ARGS__)
#define balongfb_logw(fmt, ...) \
	pr_warn("[balongfb]%s:"fmt, __func__, ##__VA_ARGS__)
#define balongfb_loge(fmt, ...) \
	pr_err("[balongfb]%s: "fmt, __func__, ##__VA_ARGS__)

/*--------------------------------------------------------------------------*/
#ifdef PC_UT_TEST_ON
#define STATIC

extern volatile unsigned int    g_pc_ut_reg_data[0xffff];
#define outp32(addr, val)       ( g_pc_ut_reg_data[(u32)addr] = val )
#define outp16(addr, val)
#define outp8(addr, val)
#define outp(addr, val)         outp32(addr, val)

#define inp32(addr)             ( g_pc_ut_reg_data[(u32)addr] )
#define inp16(addr)
#define inp8(addr)
#define inp(addr)

#define inpw(port)
#define outpw(port, val)
#define inpdw(port)
#define outpdw(port, val)
#else
#define STATIC   static

#define outp32(addr, val) writel(val, addr)
#define outp16(addr, val) writew(val, addr)
#define outp8(addr, val) writeb(val, addr)
#define outp(addr, val) outp32(addr, val)

#define inp32(addr) readl(addr)
#define inp16(addr) readw(addr)
#define inp8(addr) readb(addr)
#define inp(addr) inp32(addr)

#define inpw(port) readw(port)
#define outpw(port, val) writew(val, port)
#define inpdw(port) readl(port)
#define outpdw(port, val) writel(val, port)
#endif /* PC_UT_TEST_ON */

enum ADE_LOG_LEVEL {
    ADE_NO_PRINT    = 0,
    ADE_ERR_PRINT   = 1,
    ADE_WARN_PRINT  = 2,
    ADE_INFO_PRINT  = 3,
    ADE_DEBUG_PRINT = 4
};
typedef uint32_t ADE_LOG_LEVEL_UINT32;
#if ADE_DEBUG_LOG_ENABLE
void balongfb_logi_vsync_debugfs (const char* fmt, ...);
void balongfb_logd_display_debugfs(const char* fmt, ...);
void balongfb_logi_display_debugfs(const char* fmt, ...);
void balongfb_logw_display_debugfs(const char* fmt, ...);
void balongfb_loge_display_debugfs(const char* fmt, ...);
void balongfb_logi_backlight_debugfs(const char* fmt, ...);
void balongfb_logd_top_info(const char* fmt, ...);
void balongfb_logi_top_info(const char* fmt, ...);
void balongfb_logw_top_info(const char* fmt, ...);
void balongfb_loge_top_info(const char* fmt, ...);

#else
#define balongfb_logi_vsync_debugfs(fmt, ...)
#define balongfb_loge_display_debugfs(fmt, ...)
#define balongfb_logw_display_debugfs(fmt, ...)
#define balongfb_logi_display_debugfs(fmt, ...)
#define balongfb_logd_display_debugfs(fmt, ...)
#define balongfb_logi_backlight_debugfs(fmt, ...)
#define balongfb_loge_top_info(fmt, ...)
#define balongfb_logw_top_info(fmt, ...)
#define balongfb_logi_top_info(fmt, ...)
#define balongfb_logd_top_info(fmt, ...)




#endif /* CONFIG_DEBUG_FS */

#endif /* __BALONG_FB_DEF_H__ */
