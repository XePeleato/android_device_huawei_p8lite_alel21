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
#ifndef __HI6402ES_DSP_H__
#define __HI6402ES_DSP_H__

#ifndef OK
#define OK            0
#endif
#define ERROR       (-1)
#define BUSY        (-2)
#define NOMEM       (-3)
#define INVALID     (-4)

#define MSG_SEND_RETRIES 3

#define INT_TO_ADDR(low,high) (void*) (unsigned long)((unsigned long long)(low) | ((unsigned long long)(high)<<32))
#define GET_LOW32(x) (unsigned int)(((unsigned long long)(unsigned long)(x))&0xffffffffULL)
#define GET_HIG32(x) (unsigned int)((((unsigned long long)(unsigned long)(x))>>32)&0xffffffffULL)

struct misc_io_async_param {
	unsigned int para_in_l;
	unsigned int para_in_h;
	unsigned int para_size_in;
};

struct misc_io_sync_param {
	unsigned int para_in_l;
	unsigned int para_in_h;
	unsigned int para_size_in;

	unsigned int para_out_l;
	unsigned int para_out_h;
	unsigned int para_size_out;
};

#define HI6402ES_HIFI_MISC_IOCTL_ASYNCMSG        _IOWR('A', 0x90, struct misc_io_async_param)
#define HI6402ES_HIFI_MISC_IOCTL_SYNCMSG         _IOWR('A', 0x91, struct misc_io_sync_param)


/*
 *  dsp img download
 */
enum DRV_HIFI_IMAGE_SEC_TYPE_ENUM {
	DRV_HIFI_IMAGE_SEC_TYPE_CODE = 0,                /* code section */
	DRV_HIFI_IMAGE_SEC_TYPE_DATA,                    /* data section */
	DRV_HIFI_IMAGE_SEC_TYPE_BSS,                     /* bss section */
	DRV_HIFI_IMAGE_SEC_TYPE_BUTT,
};
enum DRV_HIFI_IMAGE_SEC_LOAD_ENUM {
	DRV_HIFI_IMAGE_SEC_LOAD_STATIC = 0,             /* before dsp reset  download one time*/
	DRV_HIFI_IMAGE_SEC_LOAD_DYNAMIC,                /* maybe need download dynamic */
	DRV_HIFI_IMAGE_SEC_UNLOAD,                      /* the section do not need download*/
	DRV_HIFI_IMAGE_SEC_LOAD_BUTT,
};

struct drv_hifi_image_sec {
	unsigned short	sn;                            /* section serial number*/
	unsigned char	type;                          /* section type :code, data, bss    */
	unsigned char	load_attib;                    /* download attribute:static,dynmic,unload*/
	unsigned int	src_offset;                    /* offset of down file, bytes */
	unsigned int	des_addr;                      /* des addr , bytes */
	unsigned int	size;                          /* section length, bytes */
};

struct drv_hifi_image_head {
	char				time_stamp[24];            /* image time stamp */
	unsigned int			image_size;            /* image size, bytes */
	unsigned int			sections_num;          /* section number */
	struct drv_hifi_image_sec	sections[32];      /* section head include section infomation */
};

enum HI6402ES_HIFI_PCM_SAMPLE_RATE {
	HI6402ES_HIFI_PCM_SAMPLE_RATE_8K = 0,
	HI6402ES_HIFI_PCM_SAMPLE_RATE_16K,
	HI6402ES_HIFI_PCM_SAMPLE_RATE_32K,
	HI6402ES_HIFI_PCM_SAMPLE_RATE_RESERVED0,
	HI6402ES_HIFI_PCM_SAMPLE_RATE_48K,
	HI6402ES_HIFI_PCM_SAMPLE_RATE_96K,
	HI6402ES_HIFI_PCM_SAMPLE_RATE_192K,
	HI6402ES_HIFI_PCM_SAMPLE_RATE_RESERVED1,
};

enum HI6402ES_HIFI_PCM_DIRECT {
	HI6402ES_HIFI_PCM_IN = 0,
	HI6402ES_HIFI_PCM_OUT,
	HI6402ES_HIFI_PCM_DIRECT_BUTT,
};

struct krn_param_io_buf {
	unsigned char *buf_in;
	unsigned int buf_size_in;
	/*XXX: variables below is for sync cmd only*/
	unsigned char *buf_out;
	unsigned int buf_size_out;
};

typedef int (*cmd_process_func)(struct krn_param_io_buf *);

struct cmd_func_pair {
	int cmd_id;
	cmd_process_func func;
};


#endif/*__HI6402ES_DSP_H__*/
