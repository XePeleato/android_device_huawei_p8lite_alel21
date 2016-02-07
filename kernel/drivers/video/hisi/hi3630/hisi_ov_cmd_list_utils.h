/* Copyright (c) 2013-2014, Hisilicon Tech. Co., Ltd. All rights reserved.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 and
* only version 2 as published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
* GNU General Public License for more details.
*
*/
#ifndef _CMD_LIST_UTILS_H_
#define _CMD_LIST_UTILS_H_


typedef union {
	struct {
		uint32_t exec:1;
		uint32_t last:1;
		uint32_t nop:1;
		uint32_t interrupt:1;
		uint32_t pending:1;
		uint32_t id:10;
		uint32_t reserved:9;
		uint32_t valid_flag:8; //0xA5 is valid
	} bits;
	uint32_t ul32;

} cmd_flag_t;

typedef union {
	struct {
		uint32_t count:14;
		uint32_t reserved:18;
	} bits;
	uint32_t ul32;

} total_items_t;

typedef union {
	struct {
		uint32_t add0:18;
		uint32_t add1:6;
		uint32_t add2:6;
		uint32_t cnt:2;
	} bits;
	uint32_t ul32;

} reg_addr_t;

typedef struct cmd_item {
	reg_addr_t reg_addr;
	uint32_t data0;
	uint32_t data1;
	uint32_t data2;

} cmd_item_t;

typedef struct cmd_header {
	cmd_flag_t   flag;
	uint32_t    next_list;
	total_items_t    total_items;
	uint32_t    list_addr; //128bit align

} cmd_header_t;

enum dss_cmdlist_status {
	e_status_idle = 0x0,
	e_status_wait = 0x1,
	e_status_other,
};

enum dss_cmdlist_node_type {
	e_node_none = 0x0,
	e_node_first_nop = 0x1,
	e_node_nop = 0x2,
	e_node_frame = 0x3,
};

enum dss_cmdlist_list_valid {
	e_list_invalid = 0x0,
	e_list_valid = 0xA5,
};


/*
*for normal node,all variable should be filled.
*for NOP node, just the list_header,header_ion_handle,list_node, list_node_flag should be filled.
*list_node_flag must be e_node_nop when it is NOP node.
*And item_ion_handle in NOP node should be NULL.
*/
typedef struct dss_cmdlist_node {
	cmd_header_t * list_header;
	cmd_item_t *list_item;
	uint32_t item_index;
	int item_flag;

	int is_used;
	struct ion_handle* header_ion_handle;
	ion_phys_addr_t header_phys;
	struct ion_handle* item_ion_handle;
	ion_phys_addr_t item_phys;
	struct list_head list_node;

	uint32_t list_node_flag;
	uint32_t list_node_type;

} dss_cmdlist_node_t;


int cmdlist_alloc_one_list(dss_cmdlist_node_t ** cmdlist_node, struct ion_client *ion_client);
int cmdlist_free_one_list(dss_cmdlist_node_t * cmdlist_node, struct ion_client *ion_client);
dss_cmdlist_node_t * cmdlist_get_one_list(struct hisi_fb_data_type *hisifd, int * id);
int cmdlist_throw_one_list(dss_cmdlist_node_t * cmdlist_node);

void ov_cmd_list_output_32(struct hisi_fb_data_type *hisifd, char __iomem *base_addr, uint32_t value, uint8_t bw, uint8_t bs);

int cmdlist_add_new_list(struct hisi_fb_data_type *hisifd, struct list_head * cmdlist_head, int is_last, uint32_t flag);
void cmdlist_frame_valid(struct list_head * cmdlist_head);
void cmdlist_flush_cmdlistmemory_cache(struct list_head * cmdlist_head, struct ion_client *ion_client);

int cmdlist_add_nop_list(struct hisi_fb_data_type *hisifd, struct list_head *cmdlist_head, int is_first, int is_last);
void cmdlist_start_frame(struct hisi_fb_data_type *hisifd, uint32_t wbe_chn);
void cmdlist_free_nop(struct list_head * cmdlist_head, struct ion_client *ion_client);
void cmdlist_free_frame(struct list_head * cmdlist_head, struct ion_client *ion_client);

int cmdlist_prepare_next_frame(struct hisi_fb_data_type *hisifd, uint32_t wbe_chn);

int cmdlist_config_start(struct hisi_fb_data_type *hisifd, uint32_t wbe_chn);
int cmdlist_config_stop(struct hisi_fb_data_type *hisifd, uint32_t wbe_chn);

void cmdlist_print_all_node(struct list_head * cmdlist_head);
void cmdlist_print_node_items(cmd_item_t * item, uint32_t count);


#endif
