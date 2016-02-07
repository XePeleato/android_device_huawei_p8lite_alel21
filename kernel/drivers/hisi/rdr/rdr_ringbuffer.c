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
#include <linux/huawei/hisi_ringbuffer.h>

void hisi_ringbuffer_init(struct hisi_ringbuffer_s *q, u32 bytes, u32 fieldcnt)
{
	/* max_num: records count. */
	q->max_num = (bytes - sizeof(struct hisi_ringbuffer_s)) /
					(sizeof(u32) * fieldcnt);
	q->rear = 0; /* point to the last NULL record. UNIT is record. */
	q->is_full = 0;
	q->field_count = fieldcnt; /* How many u32 in ONE record. */
}

/*lint -e818 */
void hisi_ringbuffer_write(struct hisi_ringbuffer_s *q, u32 *element)
{
	int rear;

	if (q->rear >= q->max_num) {
		q->is_full = 1;
		q->rear = 0;
	}

/*lint -e713 */
	rear = q->rear++;
/*lint +e713 */

/*lint -e737 */
	memcpy((void *)&q->data[rear * q->field_count],
		(void *)element, q->field_count * sizeof(u32));
/*lint +e737 */
	return;
}
/*lint +e818 */
