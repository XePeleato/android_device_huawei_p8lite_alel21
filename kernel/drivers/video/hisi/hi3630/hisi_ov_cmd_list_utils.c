#include "hisi_fb.h"

#define MAX_ITEM_OFFSET ( 0x3F )
#define CMDLIST_ADDR_OFFSET (0x3FFFF)

#define CMDLIST_HEADER_LEN (SZ_1K)
#define CMDLIST_ITEM_LEN (SZ_16K)
#define MAX_ITEM_INDEX (SZ_1K)


#define	ERR_RETURN(condition) \
		if(condition) \
			return -1;

/*
**set one register into cmdlist.
*/
void ov_cmd_list_output_32(struct hisi_fb_data_type *hisifd, char __iomem *base_addr,
	uint32_t value, uint8_t bw, uint8_t bs)
{
	int index;
	uint64_t item_addr;
	uint32_t mask = (1 << bw) - 1;
	dss_cmdlist_node_t * cmdlist_node = NULL;
	uint32_t wb_chn;

	BUG_ON(base_addr == NULL);
	BUG_ON(hisifd == NULL);

	wb_chn = hisifd->ov_req.wb_layer_info.chn_idx - WBE1_CHN0;
	if (wb_chn  >= HISI_DSS_OFFLINE_MAX_NUM ) {
		HISI_FB_ERR("not support, wbe_chn=%d!\n", wb_chn);
		return;
	}

	/*get the last list*/
	cmdlist_node = list_entry(hisifd->offline_cmdlist_head[wb_chn].prev, dss_cmdlist_node_t, list_node);
	//HISI_FB_ERR("list header_addr:0x%p\n", cmdlist_node->list_header);
	BUG_ON(cmdlist_node == NULL);
	if (cmdlist_node->list_node_type== e_node_nop) {
		HISI_FB_ERR("can't set register value to NOP node!\n");
		return;
	}

	index = cmdlist_node->item_index;
	item_addr = (uint64_t) base_addr >> 2;
	item_addr = item_addr & CMDLIST_ADDR_OFFSET;

	do {
		uint32_t old_addr= cmdlist_node->list_item[index].reg_addr.ul32 & CMDLIST_ADDR_OFFSET;
		uint32_t offset = item_addr - old_addr;
		int condition = ((offset < MAX_ITEM_OFFSET) && (item_addr >= old_addr));

		if(cmdlist_node->item_flag == 3) {
			index++;
			cmdlist_node->item_flag = 0;
		}

		if (bw != 32) {
			if(cmdlist_node->item_flag != 0)
				index++;

			cmdlist_node->list_item[index].reg_addr.bits.add0 = item_addr;
			cmdlist_node->list_item[index].data0 = value;
			cmdlist_node->list_item[index].data1 = ~(mask << bs);
			cmdlist_node->list_item[index].data2 = (mask & value) << bs;
			cmdlist_node->list_item[index].reg_addr.bits.cnt = 3;

			cmdlist_node->item_flag = 3;
			//HISI_FB_ERR("should not set %d bits,data:0x%x addr:0x%x \n", bw, value, (uint32_t)base_addr);
			break;
		}

		if (cmdlist_node->item_flag == 0) {
			//HISI_FB_ERR("%d set %d bits,old_addr:0x%x item_addr:0x%x offset:%d \n", cmdlist_node->item_flag, bw, old_addr, (uint32_t)item_addr, offset);
			cmdlist_node->list_item[index].reg_addr.bits.add0 = item_addr;
			cmdlist_node->list_item[index].data0 = value;
			cmdlist_node->list_item[index].reg_addr.bits.cnt = 0;
			cmdlist_node->item_flag = 1;
			break;
		}

		if (cmdlist_node->item_flag == 1 && condition) {
			//HISI_FB_ERR("%d set %d bits,old_addr:0x%x item_addr:0x%x offset:%d \n", cmdlist_node->item_flag, bw, old_addr, (uint32_t)item_addr, offset);
			cmdlist_node->list_item[index].reg_addr.bits.add1 = offset;
			cmdlist_node->list_item[index].data1 = value;
			cmdlist_node->list_item[index].reg_addr.bits.cnt = 1;
			cmdlist_node->item_flag = 2;
			break;
		}

		if (cmdlist_node->item_flag == 2 && condition) {
			//HISI_FB_ERR("%d set %d bits,old_addr:0x%x item_addr:0x%x offset:%d \n", cmdlist_node->item_flag, bw, old_addr, (uint32_t)item_addr, offset);
			cmdlist_node->list_item[index].reg_addr.bits.add2 = offset;
			cmdlist_node->list_item[index].data2 = value;
			cmdlist_node->list_item[index].reg_addr.bits.cnt = 2;
			cmdlist_node->item_flag = 3;
			break;
		}

		//HISI_FB_ERR("%d set %d bits,old_addr:0x%x item_addr:0x%x offset:%d \n", cmdlist_node->item_flag, bw, old_addr, (uint32_t)item_addr, offset);
		index++;
		cmdlist_node->list_item[index].reg_addr.bits.add0 = item_addr;
		cmdlist_node->list_item[index].data0 = value;
		cmdlist_node->list_item[index].reg_addr.bits.cnt = 0;
		cmdlist_node->item_flag = 1;
	} while (0);

	BUG_ON(index >= MAX_ITEM_INDEX);
	cmdlist_node->item_index = index;
	cmdlist_node->list_header->total_items.bits.count= index;
}

int cmdlist_alloc_one_list(dss_cmdlist_node_t ** cmdlist_node, struct ion_client *ion_client)
{
	int ret;
	dss_cmdlist_node_t * cmdlist = NULL;
	size_t header_len = CMDLIST_HEADER_LEN;
	size_t item_len = CMDLIST_ITEM_LEN;

	cmdlist = (dss_cmdlist_node_t *)kzalloc(sizeof(dss_cmdlist_node_t), GFP_KERNEL);
	ERR_RETURN(IS_ERR_OR_NULL(cmdlist));
	memset(cmdlist, 0, sizeof(dss_cmdlist_node_t));

	/*alloc buffer for header*/
	cmdlist->header_ion_handle = ion_alloc(ion_client, header_len, 0, ION_HEAP(ION_GRALLOC_HEAP_ID), 0);
	ERR_RETURN(IS_ERR_OR_NULL(cmdlist->header_ion_handle));

	cmdlist->list_header = (cmd_header_t *)ion_map_kernel(ion_client, cmdlist->header_ion_handle);
	ERR_RETURN(IS_ERR_OR_NULL(cmdlist->list_header));

	memset(cmdlist->list_header, 0, header_len);

	ret = ion_phys(ion_client, cmdlist->header_ion_handle, &cmdlist->header_phys, &header_len);
	ERR_RETURN(ret < 0);

	/*alloc buffer for items*/
	cmdlist->item_ion_handle = ion_alloc(ion_client, item_len, 0, ION_HEAP(ION_GRALLOC_HEAP_ID), 0);
	ERR_RETURN(IS_ERR_OR_NULL(cmdlist->item_ion_handle));

	cmdlist->list_item = (cmd_item_t *)ion_map_kernel(ion_client, cmdlist->item_ion_handle);
	ERR_RETURN(IS_ERR_OR_NULL(cmdlist->list_item));

	memset(cmdlist->list_item, 0, item_len);

	ret = ion_phys(ion_client, cmdlist->item_ion_handle, &cmdlist->item_phys, &item_len);
	ERR_RETURN(ret < 0);

	*cmdlist_node = cmdlist;
	cmdlist->is_used = FALSE;
	cmdlist->item_flag= 0;
	cmdlist->item_index= 0;
	cmdlist->list_node_flag = 0;
	cmdlist->list_node_type = e_node_none;

	return ret;
}

int cmdlist_free_one_list(dss_cmdlist_node_t * cmdlist_node, struct ion_client *ion_client)
{
	ion_free(ion_client, cmdlist_node->header_ion_handle);
	ion_free(ion_client, cmdlist_node->item_ion_handle);
	kfree(cmdlist_node);

	return 0;
}

dss_cmdlist_node_t * cmdlist_get_one_list(struct hisi_fb_data_type *hisifd, int * id)
{
	int i = 0;

	for(i = 0; i < HISI_DSS_OFFLINE_MAX_LIST; i++){
		if(hisifd->cmdlist_node[i]->is_used == FALSE){
			hisifd->cmdlist_node[i]->is_used = TRUE;
			*id = i + 1;
			return hisifd->cmdlist_node[i];
		}
	}

	return NULL;
}

int cmdlist_throw_one_list(dss_cmdlist_node_t * cmdlist_node)
{
	cmdlist_node->is_used = FALSE;
	cmdlist_node->item_flag= 0;
	cmdlist_node->item_index= 0;
	cmdlist_node->list_node_flag = 0;
	cmdlist_node->list_node_type = e_node_none;

	memset(cmdlist_node->list_header, 0, CMDLIST_HEADER_LEN);
	memset(cmdlist_node->list_item, 0, CMDLIST_ITEM_LEN);

	return 0;
}


int cmdlist_add_nop_list(struct hisi_fb_data_type *hisifd, struct list_head *cmdlist_head, int is_first, int is_last)
{
	dss_cmdlist_node_t * cmdlist_node = NULL;
	int ret = 0;
	int id = 0;

	BUG_ON(hisifd == NULL);
	BUG_ON(cmdlist_head == NULL);

	cmdlist_node = cmdlist_get_one_list(hisifd, &id);
	ERR_RETURN(cmdlist_node == NULL);

	cmdlist_node->list_header->flag.bits.id = id;
	cmdlist_node->list_header->flag.bits.nop = 0x01;
	cmdlist_node->list_header->flag.bits.last = is_last ? 1 : 0;
	cmdlist_node->list_header->flag.bits.pending = (is_first == TRUE) ? 0x01 : 0;
	cmdlist_node->list_header->flag.bits.valid_flag = e_list_valid;
	cmdlist_node->list_header->next_list = 0xFFFFFFFF;

	cmdlist_node->list_node_flag = (is_first == TRUE) ? cmdlist_node->header_phys : 0;
	cmdlist_node->list_node_type = (is_first == TRUE) ? e_node_first_nop : e_node_nop;

	/*add this nop to list*/
	list_add_tail(&cmdlist_node->list_node, cmdlist_head);

	if (cmdlist_node->list_node.prev != cmdlist_head) {
		dss_cmdlist_node_t * pre_node = NULL;
		pre_node = list_entry(cmdlist_node->list_node.prev, dss_cmdlist_node_t, list_node);
		pre_node->list_header->next_list = cmdlist_node->header_phys;
		//HISI_FB_ERR("next_list = 0x%x\n", (uint32_t)header_phys);
	}

	return ret;
}

/*
**add one list of the frame list, but leave the valid flag blank.
**if this list will be executed,call cmdlist_frame_valid().
*/
int cmdlist_add_new_list(struct hisi_fb_data_type *hisifd, struct list_head * cmdlist_head, int is_last, uint32_t flag)
{
	dss_cmdlist_node_t * cmdlist_node = NULL;
	int ret = 0;
	int id = 0;

	BUG_ON(hisifd == NULL);
	BUG_ON(cmdlist_head == NULL);

	cmdlist_node = cmdlist_get_one_list(hisifd, &id);
	ERR_RETURN(cmdlist_node == NULL);

	/*fill the header and item info*/
	cmdlist_node->list_header->flag.bits.id = id;
	cmdlist_node->list_header->flag.bits.pending = (is_last == TRUE) ? 0x01 : 0;

	/*valid_flag will be set 0xA5 in cmdlist_frame_valid()*/
	cmdlist_node->list_header->flag.bits.valid_flag = e_list_invalid;
	cmdlist_node->list_header->flag.bits.exec = 0x01;

	cmdlist_node->list_header->list_addr = cmdlist_node->item_phys;
	cmdlist_node->list_header->next_list = 0xFFFFFFFF;

	cmdlist_node->list_node_flag = flag;
	cmdlist_node->list_node_type = e_node_frame;
	cmdlist_node->item_flag = 0;

	/*add this nop to list*/
	list_add_tail(&cmdlist_node->list_node, cmdlist_head);

	if (cmdlist_node->list_node.prev != cmdlist_head) {
		dss_cmdlist_node_t * pre_node = NULL;
		pre_node = list_entry(cmdlist_node->list_node.prev, dss_cmdlist_node_t, list_node);
		pre_node->list_header->next_list = cmdlist_node->header_phys;
		//HISI_FB_ERR("is_last:%d next_list = 0x%x\n", is_last, (uint32_t)header_phys);
	}

	return ret;
}

/*
**ensure the the frame list before, set their valid flag to 0xA5.
*/
void cmdlist_frame_valid(struct list_head * cmdlist_head)
{
	dss_cmdlist_node_t *node, *_node_;

	BUG_ON(cmdlist_head == NULL);
	list_for_each_entry_safe_reverse(node, _node_, cmdlist_head, list_node) {
		if (node->list_node_type == e_node_nop)
			break;

		node->list_header->flag.bits.valid_flag = e_list_valid;
		node->list_header->total_items.bits.count++;
	}
}

/*
**flush cache for cmdlist, make sure that cmdlist has writen through to memory before config register
*/
void cmdlist_flush_cmdlistmemory_cache(struct list_head * cmdlist_head, struct ion_client *ion_client)
{
	dss_cmdlist_node_t *node, *_node_;
	struct sg_table * table;

	list_for_each_entry_safe_reverse(node, _node_, cmdlist_head, list_node) {
		//flush cache for header
		table = ion_sg_table(ion_client, node->header_ion_handle);
		BUG_ON(table == NULL);
		dma_sync_sg_for_device(NULL, table->sgl, table->nents, DMA_TO_DEVICE);
		//flush cache for item
		table = ion_sg_table(ion_client, node->item_ion_handle);
		BUG_ON(table == NULL);
		dma_sync_sg_for_device(NULL, table->sgl, table->nents, DMA_TO_DEVICE);
	}

	return;
}

/*
**free the NOP list in the cmdlist.
*/
void cmdlist_free_nop(struct list_head * cmdlist_head, struct ion_client *ion_client)
{
	dss_cmdlist_node_t * nop_node;
	dss_cmdlist_node_t * first_nop_node;

	BUG_ON(cmdlist_head == NULL);
	BUG_ON(ion_client == NULL);

	/*delete the nop node before frame node.*/
	nop_node = list_first_entry(cmdlist_head, dss_cmdlist_node_t, list_node);
	list_del(&nop_node->list_node);

	if (nop_node->list_node_type == e_node_first_nop) {

		/*delete the nop after the first one*/
		first_nop_node = list_first_entry(cmdlist_head, dss_cmdlist_node_t, list_node);
		list_del(&first_nop_node->list_node);
		cmdlist_throw_one_list(first_nop_node);
	}

	cmdlist_throw_one_list(nop_node);
}

/*
**free one frame list(it posiblely contain some lists.) in the cmdlist.
*/
void cmdlist_free_frame(struct list_head * cmdlist_head, struct ion_client *ion_client)
{
	dss_cmdlist_node_t *nop_node, *normal_node;

	BUG_ON(cmdlist_head == NULL);
	BUG_ON(ion_client == NULL);

	list_for_each_entry_safe(normal_node, nop_node, cmdlist_head, list_node) {
		if (normal_node->list_node_type <= e_node_nop)
			break;

		list_del(&normal_node->list_node);
		cmdlist_throw_one_list(normal_node);
	}
}

/*
**as the function name says.
*/
static int check_if_have_another_frame(struct list_head * cmdlist_head)
{
	dss_cmdlist_node_t *node, *_node_;
	int node_count = 0;

	list_for_each_entry_safe(node, _node_, cmdlist_head, list_node) {
		if (node->list_node_type == e_node_nop) {
			node_count++;

			if (node_count > 2)
				return TRUE;
		}
	}

	return FALSE;
}

/*
**set the cmdlist after one frame end interrupt.
*/
int cmdlist_prepare_next_frame(struct hisi_fb_data_type *hisifd, uint32_t wbe_chn)
{
	BUG_ON(hisifd == NULL);
	BUG_ON(wbe_chn >= HISI_DSS_OFFLINE_MAX_NUM);

	if (check_if_have_another_frame(&hisifd->offline_cmdlist_head[wbe_chn])) {

		cmdlist_start_frame(hisifd, wbe_chn);
		//printk(KERN_ERR "cmdlist_prepare_next_frame has next frame wbe_chn:%d \n",wbe_chn);
	} else {
		hisifd->offline_wb_status[wbe_chn] = e_status_wait;
	}

	return 0;
}

/*
**stop the pending state for one new frame if the current cmdlist status is e_status_wait.
*/
void cmdlist_start_frame(struct hisi_fb_data_type *hisifd, uint32_t wbe_chn)
{
	char __iomem *cmd_list_base = hisifd->dss_base + DSS_CMD_LIST_OFFSET;
	uint32_t offset = CMDLIST_CH5_CTRL - CMDLIST_CH4_CTRL;
	uint32_t status = 0;

	BUG_ON(hisifd == NULL);
	BUG_ON(wbe_chn >= HISI_DSS_OFFLINE_MAX_NUM);

	if (hisifd->offline_wb_status[wbe_chn] == e_status_wait) {

		status = inp32(cmd_list_base + CMDLIST_CH4_STATUS + wbe_chn * offset);
		if((status & 0x07) != 0x07){
			printk(KERN_ERR "enter cmdlist_start_frame status:0x%x \n",status);
		}

		if(wbe_chn){
			set_reg(cmd_list_base + CMDLIST_CH5_INTC, 0x1, 1, 3);
		}else{
			set_reg(cmd_list_base + CMDLIST_CH4_INTC, 0x1, 1, 3);
		}

		set_reg(cmd_list_base + CMDLIST_CH4_INTE + wbe_chn * offset, 0x08, 6, 0);

		/* stop pending*/
		set_reg(cmd_list_base + CMDLIST_CH4_CTRL + wbe_chn * offset, 0x01, 1, 2);
		set_reg(cmd_list_base + CMDLIST_CH4_CTRL + wbe_chn * offset, 0x0, 1, 2);

		status = inp32(cmd_list_base + CMDLIST_CH4_STATUS + wbe_chn * offset);
		if((status & 0x07) == 0x07){
			printk(KERN_ERR "exit cmdlist_start_frame status:0x%x \n",status);
		}

	}

	return;
}

/*
**start cmdlist.
**it will set cmdlist into pending state.
*/
int cmdlist_config_start(struct hisi_fb_data_type *hisifd, uint32_t wbe_chn)
{
	char __iomem *cmd_list_base = hisifd->dss_base + DSS_CMD_LIST_OFFSET;
	dss_cmdlist_node_t * cmdlist_node = NULL;
	uint32_t list_addr;
	uint32_t offset = CMDLIST_CH5_CTRL - CMDLIST_CH4_CTRL;
	uint32_t status = 0;
	uint32_t try_times = 0;

	BUG_ON(hisifd == NULL);
	BUG_ON(wbe_chn >= HISI_DSS_OFFLINE_MAX_NUM);

	cmdlist_add_nop_list(hisifd, &hisifd->offline_cmdlist_head[wbe_chn], TRUE, FALSE);
	cmdlist_add_nop_list(hisifd, &hisifd->offline_cmdlist_head[wbe_chn], FALSE, FALSE);

	cmdlist_node = list_first_entry(&hisifd->offline_cmdlist_head[wbe_chn], dss_cmdlist_node_t, list_node);
	list_addr = cmdlist_node->list_node_flag;
	cmdlist_node->list_node_flag = 0;

	cmdlist_flush_cmdlistmemory_cache(&hisifd->offline_cmdlist_head[wbe_chn], hisifd->ion_client);

	/*ch4_start_addr */
	set_reg(cmd_list_base + CMDLIST_CH4_STAAD + wbe_chn * offset, list_addr, 32, 0);
	//set_reg(cmd_list_base + 0xAC, 2, 2, 8);
	//set_reg(cmd_list_base + 0xB8, 1, 1, 4);

	/*chx_start*/ /*chx_enable*/
	set_reg(cmd_list_base + CMDLIST_CH4_CTRL + wbe_chn * offset, 0x21, 6, 0);
	/* disable cmdlist irq */
	set_reg(cmd_list_base + CMDLIST_CH4_INTE + wbe_chn * offset, 0, 6, 0);

	status = inp32(cmd_list_base + CMDLIST_CH4_STATUS + wbe_chn * offset);
	while ((status & 0x07) != 0x07) {
		udelay(10);

		if (++try_times > 100) {
			try_times = 0;
			HISI_FB_ERR("cmdlist_config_start check CMDLIST_CH4_STATUS(0x%x) timeout!\n",
				status);
			break;
		}

		status = inp32(cmd_list_base + CMDLIST_CH4_STATUS + wbe_chn * offset);
	}

	return 0;
}

/*
**free all cmdlist in buffer, it will be called before stop cmdlist.
*/
static void cmdlist_free_all(struct list_head * cmdlist_head, struct ion_client *ion_client)
{
	dss_cmdlist_node_t * node, *normal_node;

	BUG_ON(ion_client == NULL);
	BUG_ON(cmdlist_head == NULL);

	list_for_each_entry_safe(normal_node, node, cmdlist_head, list_node) {

		list_del(&normal_node->list_node);
		cmdlist_throw_one_list(normal_node);
	}
}

/*
**stop cmdlist.
**it will set cmdlist into idle state.
*/
int cmdlist_config_stop(struct hisi_fb_data_type *hisifd, uint32_t wbe_chn)
{
	char __iomem *cmd_list_base = hisifd->dss_base + DSS_CMD_LIST_OFFSET;
	uint32_t status = 0;
	int count = 0;
	uint32_t offset = CMDLIST_CH5_CTRL - CMDLIST_CH4_CTRL;

	BUG_ON(hisifd == NULL);
	BUG_ON(wbe_chn >= HISI_DSS_OFFLINE_MAX_NUM);

	set_reg(cmd_list_base + CMDLIST_CH4_CTRL + wbe_chn * offset, 1, 1, 1);

	while ((status & BIT_CHX_IDLE) == 0 && count++ <= 50) {
		status = inp32(cmd_list_base + CMDLIST_CH4_STATUS + wbe_chn * offset);
		udelay(1);
	}

	if(count >= 50)
		HISI_FB_ERR("cmdlist_config_stop errr, can not exit to idle!\n");

	set_reg(cmd_list_base + CMDLIST_SWRST, 1, 1, 4 + wbe_chn);
	set_reg(cmd_list_base + CMDLIST_CH4_CTRL + wbe_chn * offset, 0x0, 2, 0);

	cmdlist_free_all(&hisifd->offline_cmdlist_head[wbe_chn], hisifd->ion_client);

	return 0;
}

void cmdlist_print_node_items(cmd_item_t * item, uint32_t count)
{
	uint32_t index = 0;
	uint32_t addr = 0;

	for (index = 0; index < count; index++) {
		addr = item[index].reg_addr.bits.add0;
		addr = addr & CMDLIST_ADDR_OFFSET;
		addr = addr << 2;
		HISI_FB_INFO("set addr:0x%x value:0x%x add1:0x%x value:0x%x add2:0x%x value:0x%x \n",
			addr, item[index].data0,
			item[index].reg_addr.bits.add1 << 2, item[index].data1,
			item[index].reg_addr.bits.add2 <<2 , item[index].data2);
	}
}

void cmdlist_print_one_node(dss_cmdlist_node_t * node)
{
	BUG_ON(node == NULL);

	if (node->list_node_type == e_node_first_nop) {
		HISI_FB_INFO("node type = fisrt NOP node\n");
	} else if (node->list_node_type == e_node_nop ) {
		HISI_FB_INFO("node type = NOP node\n");
	} else {
		HISI_FB_INFO("node type = Frame node, flag = 0x%x\n", node->list_node_flag);
	}

	HISI_FB_INFO("\t flag | pending | list_addr  | next_list  | count      |  id        \n");
	HISI_FB_INFO("\t------+---------+------------+------------+------------+------------\n");
	HISI_FB_INFO("\t 0x%2x | 0x%5x | 0x%8x | 0x%8x | 0x%8x | 0x%8x \n\n",
		node->list_header->flag.bits.valid_flag, node->list_header->flag.bits.pending,
		node->list_header->list_addr, node->list_header->next_list,
		node->list_header->total_items.bits.count, node->list_header->flag.bits.id);
}

void cmdlist_print_all_node(struct list_head * cmdlist_head)
{
	dss_cmdlist_node_t * node, *__node__;

	BUG_ON(cmdlist_head == NULL);

	list_for_each_entry_safe(node, __node__, cmdlist_head, list_node) {
		//uint32_t count = node->list_header->total_items.bits.count;
		cmdlist_print_one_node(node);
		//cmdlist_print_node_items(node->list_item, count);
	}
}
