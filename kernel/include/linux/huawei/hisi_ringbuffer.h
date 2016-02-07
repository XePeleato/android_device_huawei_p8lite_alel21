/*
 * record the data to rdr. (RDR: kernel run data recorder.)
 * This file wraps the ring buffer.
 *
 * Copyright (c) 2013 Hisilicon Technologies CO., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/kernel.h>

#ifndef __HISI_RINGBUFFER_H__
#define __HISI_RINGBUFFER_H__

struct hisi_ringbuffer_s {
	u32 max_num;
	u32 field_count;
	u32 rear;
	u32 is_full;
	u64 data[1];
};

void hisi_ringbuffer_init(struct hisi_ringbuffer_s *q, u32 bytes, u32 fieldcnt);
void hisi_ringbuffer_write(struct hisi_ringbuffer_s *q, u32 *element);
#endif
