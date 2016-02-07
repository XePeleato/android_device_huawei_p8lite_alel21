/* add cmi lcd driver */
/* Copyright (c) 2009, Code HUAWEI. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Code Aurora Forum nor
 *       the names of its contributors may be used to endorse or promote
 *       products derived from this software without specific prior written
 *       permission.
 *
 * Alternatively, provided that this notice is retained in full, this software
 * may be relicensed by the recipient under the terms of the GNU General Public
 * License version 2 ("GPL") and only version 2, in which case the provisions of
 * the GPL apply INSTEAD OF those given above.  If the recipient relicenses the
 * software under the GPL, then the identification text in the MODULE_LICENSE
 * macro must be changed to reflect "GPLv2" instead of "Dual BSD/GPL".  Once a
 * recipient changes the license terms to the GPL, subsequent recipients shall
 * not relicense under alternate licensing terms, including the BSD or dual
 * BSD/GPL terms.  In addition, the following license statement immediately
 * below and between the words START and END shall also then apply when this
 * software is relicensed under the GPL:
 *
 * START
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License version 2 and only version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * END
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/err.h>

#include<linux/init.h>
#include<linux/module.h>
#include <linux/hw_lcd_common.h>

#ifndef CONFIG_CPU_FREQ
  /* query the current CPU frequency (in kHz). If zero, cpufreq couldn't detect it */
  unsigned int cpufreq_get_fb(unsigned int cpu)
  {
        return 0;
  }
 #endif



void lcd_dcm_pwr_add_timer(void)
{
	if (lcd_pwr_status.panel_power_on == 0)
        {
            del_timer(&lcd_pwr_status.lcd_dsm_t);
            lcd_pwr_status.lcd_dsm_t.function = lcd_dcm_pwr_status_handler;
            lcd_pwr_status.lcd_dsm_t.data = 0;
            lcd_pwr_status.tm_backlight.tm_year= 0;
            lcd_pwr_status.tm_backlight.tm_mon= 0;
            lcd_pwr_status.tm_backlight.tm_mday= 0;
            lcd_pwr_status.tm_backlight.tm_hour= 0;
            lcd_pwr_status.tm_backlight.tm_min= 0;
            lcd_pwr_status.tm_backlight.tm_sec= 0;
            lcd_pwr_status.tvl_backlight.tv_usec= 0;
            lcd_pwr_status.tm_lcd_on.tm_year= 0;
            lcd_pwr_status.tm_lcd_on.tm_mon= 0;
            lcd_pwr_status.tm_lcd_on.tm_mday= 0;
            lcd_pwr_status.tm_lcd_on.tm_hour= 0;
            lcd_pwr_status.tm_lcd_on.tm_min= 0;
            lcd_pwr_status.tm_lcd_on.tm_sec= 0;
            lcd_pwr_status.tvl_lcd_on.tv_usec= 0;
            lcd_pwr_status.tm_set_frame.tm_year= 0;
            lcd_pwr_status.tm_set_frame.tm_mon= 0;
            lcd_pwr_status.tm_set_frame.tm_mday= 0;
            lcd_pwr_status.tm_set_frame.tm_hour= 0;
            lcd_pwr_status.tm_set_frame.tm_min= 0;
            lcd_pwr_status.tm_set_frame.tm_sec= 0;
            lcd_pwr_status.tvl_set_frame.tv_usec= 0;
            lcd_pwr_status.tm_unblank.tm_year= 0;
            lcd_pwr_status.tm_unblank.tm_mon= 0;
            lcd_pwr_status.tm_unblank.tm_mday= 0;
            lcd_pwr_status.tm_unblank.tm_hour= 0;
            lcd_pwr_status.tm_unblank.tm_min= 0;
            lcd_pwr_status.tm_unblank.tm_sec= 0;
            lcd_pwr_status.tvl_unblank.tv_usec= 0;
            lcd_pwr_status.lcd_dsm_t.expires = jiffies + 3 * HZ;
            lcd_pwr_status.lcd_dcm_pwr_status = 0;
            add_timer(&lcd_pwr_status.lcd_dsm_t);
        }
        else if ( lcd_pwr_status.panel_power_on == 1)
        {
            del_timer(&lcd_pwr_status.lcd_dsm_t);
        }
}

/*
*
*bit 0  do unblank
*bit 1  lcd on
*bit 2  set frame
*bit 3  set backlgiht
*/
/*if did the operation the bit will be set to 1 or the bit is 0*/
void lcd_dcm_pwr_status_handler(unsigned long data)
{

    if (lcd_pwr_status.lcd_dcm_pwr_status != LCD_PWR_STAT_GOOD)
    {
        if(!dsm_client_ocuppy(lcd_dclient))
	 {
            #if defined (CONFIG_HUAWEI_DSM)
            dsm_client_record(lcd_dclient, "unblank at [%d-%d-%d]%d:%d:%d:%d\n", lcd_pwr_status.tm_unblank.tm_year + 1900, lcd_pwr_status.tm_unblank.tm_mon + 1,
                              lcd_pwr_status.tm_unblank.tm_mday, lcd_pwr_status.tm_unblank.tm_hour, lcd_pwr_status.tm_unblank.tm_min, lcd_pwr_status.tm_unblank.tm_sec, lcd_pwr_status.tvl_unblank.tv_usec);
            dsm_client_record(lcd_dclient, "lcd on at [%d-%d-%d]%d:%d:%d:%d\n", lcd_pwr_status.tm_lcd_on.tm_year + 1900, lcd_pwr_status.tm_lcd_on.tm_mon + 1,
                              lcd_pwr_status.tm_lcd_on.tm_mday, lcd_pwr_status.tm_lcd_on.tm_hour, lcd_pwr_status.tm_lcd_on.tm_min, lcd_pwr_status.tm_lcd_on.tm_sec, lcd_pwr_status.tvl_lcd_on.tv_usec);
            dsm_client_record(lcd_dclient, "set frame at [%d-%d-%d]%d:%d:%d:%d\n", lcd_pwr_status.tm_set_frame.tm_year + 1900, lcd_pwr_status.tm_set_frame.tm_mon + 1,
                              lcd_pwr_status.tm_set_frame.tm_mday, lcd_pwr_status.tm_set_frame.tm_hour, lcd_pwr_status.tm_set_frame.tm_min, lcd_pwr_status.tm_set_frame.tm_sec, lcd_pwr_status.tvl_set_frame.tv_usec);
            dsm_client_record(lcd_dclient, "set backlight at [%d-%d-%d]%d:%d:%d:%d\n", lcd_pwr_status.tm_backlight.tm_year + 1900, lcd_pwr_status.tm_backlight.tm_mon + 1,
                              lcd_pwr_status.tm_backlight.tm_mday, lcd_pwr_status.tm_backlight.tm_hour, lcd_pwr_status.tm_backlight.tm_min, lcd_pwr_status.tm_backlight.tm_sec, lcd_pwr_status.tvl_backlight.tv_usec);
	     dsm_client_notify(lcd_dclient, DSM_LCD_POWER_STATUS_ERROR_NO);
	     #else
            balongfb_loge("unblank at [%d-%d-%d]%d:%d:%d:%d\n", lcd_pwr_status.tm_unblank.tm_year + 1900, lcd_pwr_status.tm_unblank.tm_mon + 1,
                          lcd_pwr_status.tm_unblank.tm_mday, lcd_pwr_status.tm_unblank.tm_hour, lcd_pwr_status.tm_unblank.tm_min, lcd_pwr_status.tm_unblank.tm_sec, lcd_pwr_status.tvl_unblank.tv_usec);
            balongfb_loge("lcd on at [%d-%d-%d]%d:%d:%d:%d\n", lcd_pwr_status.tm_lcd_on.tm_year + 1900, lcd_pwr_status.tm_lcd_on.tm_mon + 1,
                          lcd_pwr_status.tm_lcd_on.tm_mday, lcd_pwr_status.tm_lcd_on.tm_hour, lcd_pwr_status.tm_lcd_on.tm_min, lcd_pwr_status.tm_lcd_on.tm_sec, lcd_pwr_status.tvl_lcd_on.tv_usec);
            balongfb_loge("set frame at [%d-%d-%d]%d:%d:%d:%d\n", lcd_pwr_status.tm_set_frame.tm_year + 1900, lcd_pwr_status.tm_set_frame.tm_mon + 1,
                          lcd_pwr_status.tm_set_frame.tm_mday, lcd_pwr_status.tm_set_frame.tm_hour, lcd_pwr_status.tm_set_frame.tm_min, lcd_pwr_status.tm_set_frame.tm_sec, lcd_pwr_status.tvl_set_frame.tv_usec);
            balongfb_loge("set backlight at [%d-%d-%d]%d:%d:%d:%d\n", lcd_pwr_status.tm_backlight.tm_year + 1900, lcd_pwr_status.tm_backlight.tm_mon + 1,
                          lcd_pwr_status.tm_backlight.tm_mday, lcd_pwr_status.tm_backlight.tm_hour, lcd_pwr_status.tm_backlight.tm_min, lcd_pwr_status.tm_backlight.tm_sec, lcd_pwr_status.tvl_backlight.tv_usec);

            #endif
        }
    }
}

