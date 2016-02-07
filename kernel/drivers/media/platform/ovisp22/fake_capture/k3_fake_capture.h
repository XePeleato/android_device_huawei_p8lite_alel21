/*
 *  Hisilicon K3 fake camera v4l2 driver header file
 *
 *  CopyRight (C) Hisilicon Co., Ltd.
 *	Author : qKF23847
 *  Version: 1.2
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __K3_V4L2_CAPTURE_H__
#define __K3_V4L2_CAPTURE_H__

#include <linux/list.h>
#include <linux/videodev2.h>

#include "sensor_common.h"

#define MAX_FRAME_NR        6
#define CAMERA_FLAG_READYQ  1 

/* 
 * key and value pair struct 
 * used for setting parameters
 */
enum
{
    PKEY_IN_WIDTH = 1,          /* camera sensor input width  */
    PKEY_IN_HEIGHT,             /* camera sensor input height */
    PKEY_IN_RESOLUTION,
    PKEY_START_X,               /* capture start x position   */
    PKEY_START_Y,
    PKEY_START_POS,
    PKEY_OUT_WIDTH,
    PKEY_OUT_HEIGHT,
    PKEY_OUT_RESOLUTION,
    PKEY_OUT_FMT,
    PKEY_MIRROR,
    PKEY_BRIGHT,
    PKEY_CONTRAST,
    PKEY_FRAMERATE,
    PKEY_OUT_STRIDE,
};

typedef struct _camera_info
{
    int camera_id;
    int facing;
    int orientation; 
}camera_info;

typedef struct _camera_frame_buf
{
    int     index;
    u32     phyaddr;
    u32     size;
    void*   viraddr;
    u32     flags;                  /* reference to CAMERA_FLAG_XXX */
    struct  list_head queue;
}camera_frame_buf;


typedef struct _keypair_struct
{
	int key;
	union {
		u32     uint_value;
		int     int_value;
		char    ch_value;
		int*    pint_value;
		char*   pchar_value;
		void*   pv_value;
	} value;

} keypair_struct;

struct platform_device;
/* camera data struct */
typedef struct _v4l2_ctl_struct
{
	int                     open_count;
	
    /* camera sensors */
	int                     cur_sensor;
	
    struct video_device     *video_dev;
	struct platform_device  *pdev;
	struct semaphore        busy_lock;

	char                    preview_on;
	char                    capture_on;
	char                    ipp_on;
	
	int                     preview_nr;
    int                     capture_nr;
	int                     ipp_nr;
	
    pid_t                   preview_pid;
	pid_t                   capture_pid;
	pid_t                   ipp_pid;

	/* camera frame buffer */
	camera_frame_buf        buf_capture[MAX_FRAME_NR];
	camera_frame_buf        buf_preview[MAX_FRAME_NR];
	camera_frame_buf        buf_ipp[MAX_FRAME_NR];
    
    /* buffer list for capture and preview */
    struct list_head        preview_ready_q;            /* buffers that can be used for preview */
    struct list_head        capture_ready_q;            /* buffers that can be used for capture */
    struct list_head        ipp_ready_q;                /* buffers that can be used for ipp     */
	
    struct v4l2_fract       frame_rate;

	/* format info */
	struct v4l2_format      fmt_capture;
	struct v4l2_format      fmt_preview;

	spinlock_t queue_lock;

} v4l2_ctl_struct;

#endif /*__K3_V4L2_CAPTURE_H__ */

/********************************* END ***********************************************/
