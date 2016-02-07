/*
 * rdr field (RDR: kernel run data recorder.)
 *
 * Copyright (c) 2013 Hisilicon Technologies CO., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/string.h>
#include <linux/io.h>
#include <linux/spinlock.h>

#include <linux/huawei/rdr.h>
#include <linux/huawei/rdr_private.h>
#include "rdr_internal.h"
#include <linux/vmalloc.h>

#include <linux/hisi/hi3xxx/global_ddr_map.h>

u32 *rdr_core_addr_fun(char *daddr, int core_id)
{
	char *addr = 0;
	switch (core_id) {
	case RDR_ACORE:
		addr = daddr + RDR_ACORE_OFFSET;
		break;
	case RDR_AREA_RESERVE:
		addr = daddr + RDR_AREA_RESERVE_OFFSET;
		break;
	case RDR_CCORE:
		addr = daddr + RDR_CCORE_OFFSET;
		break;
	case RDR_TEEOS:
		addr = daddr + RDR_TEEOS_OFFSET;
		break;
	case RDR_HIFI:
		addr = daddr + RDR_HIFI_OFFSET;
		break;
	case RDR_LPM3:
		addr = daddr + RDR_LPM3_OFFSET;
		break;
	case RDR_IOM3:
		addr = daddr + RDR_IOM3_OFFSET;
		break;
	default:
		break;
	}
	return (u32 *)addr;
}

s32 rdr_mem_init(void)
{
	pbb = (struct rdr_struct_s *)ioremap_wc(HISI_RESERVED_DEBUG_PHYMEM_BASE,
								RDR_PHY_SIZE);
	DUMP_LOG((u32)pbb);
	if (pbb == NULL) {
		pr_err("rdr:%s():ioremap failed\n", __func__);
		return -1;
	}

	rdr_debug("start virtual addr is 0x%08x", (u32)pbb);
	rdr_debug("nv addr:0x%08x",
			(u32)RDR_AREA_RESERVE_ADDR->ap_cp_share.content.nv);

	rdr_tmp_pbb = (struct rdr_struct_s *)vmalloc(RDR_PHY_SIZE);
	if (rdr_tmp_pbb != NULL) {
		pr_info("rdr:%s():cp reserve mem to tmp 8m\n", __func__);
		memcpy(rdr_tmp_pbb, pbb, RDR_PHY_SIZE);
	} else
		pr_err("rdr:%s():vmalloc 8m failed,dont save 8m\n", __func__);

	return 0;
}

DEFINE_SPINLOCK(rdr_register_lock);

/*the followed 2 vars are used by rdr_field_register function.
  if the old interface is deleted, they should be moved into the function. */
static int rdr_reg_number;
static int rdr_left_byte = sizeof(struct rdr_a0_struct_s)
				- sizeof(struct rdr_area_head_s);
static int rdr_is_repeat_field_id(enum rdr_field_id_e id)
{
	int i;
	struct rdr_a0_struct_s *a0 = RDR_ACORE_ADDR;
	struct rdr_field_s *f = (struct rdr_field_s *)(a0->oam_reg);

	for (i = 0; i < rdr_reg_number; i++) {
		if ((f + i)->id == id)
			return 1;
	}

	return 0;
}

int rdr_f_register_new(u32 *field, enum rdr_field_id_e id, int type, int size)
{
	struct rdr_field_s *f;
	struct rdr_a0_struct_s *a0;

	if (pbb == NULL) {
		pr_err("rdr module is not init, exec rdr_early_init now...\n");
		if (0 != rdr_early_init())
			return -1;
	}

	if ((id & RDR_MOD_NCP_MASK) != RDR_MOD_CORE_AP) {
		pr_err("rdr:field reg failed:id is illegal:Id:%d.\n", id);
		return -1;
	}

	if (size % sizeof(u32)) {
		pr_err("rdr:field size should be word align:id:%d\n", id);
		return -1;
	}

	if (size <= 0) {
		pr_err("rdr:field size should > 0. id:%d\n", id);
		return -1;
	}

	spin_lock(&rdr_register_lock);

	if (rdr_left_byte < sizeof(struct rdr_field_s) + size) {
		pr_err("rdr:regist failed:no mem left in area:id:%d\n", id);
		spin_unlock(&rdr_register_lock);
		return -1;
	}

	if (1 == rdr_is_repeat_field_id(id)) {
		pr_err("rdr:fieldid reg failed:id is used already.id:%d\n", id);
		spin_unlock(&rdr_register_lock);
		return -1;
	}

	a0 = RDR_ACORE_ADDR;

	f = (struct rdr_field_s *)&(a0->oam_reg) + rdr_reg_number;

	if (rdr_reg_number == 0) {
		f->offset = sizeof(a0->oam_reg) - size
			+ rdr_offset(struct rdr_a0_struct_s, oam_reg);
	} else
		f->offset = (f - 1)->offset - size;
	f->size = size;
	f->reserve = RDR_FIELD_TYPE(type) | RDR_FIELD_USED;

	rdr_left_byte -= (sizeof(struct rdr_field_s) + size);

	/* change field id from str to int, top 8bit is core number */
	f->id = id;

	*field = rdr_reg_number++;
	a0->head.field_number = rdr_reg_number;

	spin_unlock(&rdr_register_lock);

	return 0;
}

u8 *rdr_balong_reg_field(u32 field_id, u32 len)
{
	u32 field;
	int ret;

	ret = rdr_afreg(field, field_id, RDR_U32, len);
	if (ret != 0) {
		pr_err("rdr:balong field reg failed\n");
		return NULL;
	}
	return field_addr(u8, field);
}
