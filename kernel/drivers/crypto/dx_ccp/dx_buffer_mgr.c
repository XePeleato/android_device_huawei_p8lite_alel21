/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This software is protected by copyright, international           *
* treaties and patents, and distributed under multiple licenses.   *
* Any use of this Software as part of the Discretix CryptoCell or  *
* Packet Engine products requires a commercial license.            *
* Copies of this Software that are distributed with the Discretix  *
* CryptoCell or Packet Engine product drivers, may be used in      *
* accordance with a commercial license, or at the user's option,   *
* used and redistributed under the terms and conditions of the GNU *
* General Public License ("GPL") version 2, as published by the    *
* Free Software Foundation.                                        *
* This program is distributed in the hope that it will be useful,  *
* but WITHOUT ANY LIABILITY AND WARRANTY; without even the implied *
* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. *
* See the GNU General Public License version 2 for more details.   *
* You should have received a copy of the GNU General Public        *
* License version 2 along with this Software; if not, please write *
* to the Free Software Foundation, Inc., 59 Temple Place - Suite   *
* 330, Boston, MA 02111-1307, USA.                                 *
* Any copy or reproduction of this Software, as permitted under    *
* the GNU General Public License version 2, must include this      *
* Copyright Notice as well as any other notices provided under     *
* the said license.                                                *
********************************************************************/

#include <linux/crypto.h>
#include <linux/version.h>
#include <crypto/algapi.h>
#include <crypto/aead.h>
#include <crypto/hash.h>
#include <crypto/authenc.h>
#include <crypto/scatterwalk.h>
#include <linux/dmapool.h>
#include <linux/dma-mapping.h>
#include <linux/crypto.h>
#include <linux/module.h>

#include "dx_config.h"
#include "dx_driver.h"
#include "dx_buffer_mgr.h"
#include "dx_cipher.h"
#include "dx_aead.h"
#include "dx_hash.h"
#include "dx_aead.h"
#include "sep_lli.h"

#define LLI_MAX_NUM_OF_DATA_ENTRIES 128
#define LLI_MAX_NUM_OF_ASSOC_DATA_ENTRIES 4
#define MLLI_TABLE_MIN_ALIGNMENT		4 /*Force the MLLI table to be align to uint32 */

/* Private pointer to the driver instance */
static struct dx_drvdata *crypto_drvdata;

struct buff_mgr_handle {
	struct dma_pool* mlli_buffs_pool;
};

#define MAX_NUM_OF_SG_IN_MLLI	4
struct sg_data_array {
	int num_of_sg;
	struct scatterlist *sg_array[MAX_NUM_OF_SG_IN_MLLI];
	int nents_array[MAX_NUM_OF_SG_IN_MLLI];
	int sg_data_len_array[MAX_NUM_OF_SG_IN_MLLI];
	bool is_last[MAX_NUM_OF_SG_IN_MLLI];
};

#ifdef DX_DEBUG
const static char buf_type_string[4][6] = {" NULL\0"," DLLI\0","MDLLI\0"," MLLI\0"};

const static char* dx_get_buff_type(unsigned int type)
{
	return &buf_type_string[type][0];
}

void dump_sg(struct scatterlist *sg)
{
/*	unsigned long	page_link;
	unsigned int	offset;
	unsigned int	length;
	dma_addr_t	dma_address;
*/
	for (; sg; sg = sg_next(sg)){
		DX_LOG_ERR(" page %x offset %x length %x (%x) dma_addr %x \n",
			     (unsigned int )sg->page_link,
			     (unsigned int )sg->offset,
			     (unsigned int )sg->length,
			     (unsigned int )sg_dma_len(sg),
			     (unsigned int )sg->dma_address);
	}

}

void dump_mlli_table(uint32_t *mlli, uint32_t nents)
{
	DX_LOG_DEBUG(" base_addr  %x nents %x \n",(uint32_t)mlli, nents);
	while(nents--) {
		DX_LOG_DEBUG(" addr  %x size %x \n",
			     (unsigned int )mlli[SEP_LLI_ADDR_WORD_OFFSET],
			     (unsigned int )mlli[SEP_LLI_SIZE_WORD_OFFSET]);
		mlli +=2;
	}

}
#endif

const char dx_dir_type[4][18] = {"DMA_BIDIRECTIONAL\0","DMA_TO_DEVICE\0",
			     "DMA_FROM_DEVICE\0","DMA_NONE\0"};

static inline const char * get_dir_type(int dir)
{
	return dx_dir_type[dir];
}

static int sg_count_ents(struct scatterlist *sg_list, int nbytes, int *lbytes)
{
	int nents = 0;
	while (nbytes > 0) {
		if (sg_is_chain(sg_list)) {
			DX_LOG_ERR("Unexpected chanined entry "
				   "in sg (entry =0x%X) \n", nents);
			BUG();
		}
		nents++;
		/* get the number of bytes in the last entry */
		*lbytes = nbytes;
		nbytes -= sg_list->length;
		sg_list = sg_next(sg_list);
	}
	DX_LOG_DEBUG("nents %d last bytes %d\n",nents, *lbytes);
	return nents;
}


/* Copy sg data, from to_skip to end, to dest and vice versa*/
void dx_sg_copy_part(u8 *dest, struct scatterlist *sg,
	      int to_skip, unsigned int end, enum dx_sg_cpy_direct direct)
{
	struct scatterlist t_sg;
	struct scatterlist *current_sg = sg;
	int sg_index, cpy_index;
	int nents;
	int lbytes;

	nents = sg_count_ents(sg, end, &lbytes);
	sg_index = current_sg->length;
	while (sg_index <= to_skip) {
		current_sg = scatterwalk_sg_next(current_sg);
		sg_index += current_sg->length;
		nents--;
	}
	cpy_index = sg_index - to_skip;
	/* copy current sg to temporary */
	t_sg = *current_sg;
	/*update the offset in the sg entry*/
	t_sg.offset += current_sg->length - cpy_index;
	/*copy the data*/
	if (direct == DX_SG_TO_BUF) {
		sg_copy_to_buffer(&t_sg, 1, dest, cpy_index);
	} else {
		sg_copy_from_buffer(&t_sg, 1, dest, cpy_index);
	}
	current_sg = scatterwalk_sg_next(current_sg);
	nents--;

	if (end > sg_index) {
		if (direct == DX_SG_TO_BUF) {
			sg_copy_to_buffer(current_sg, nents,
					  &dest[cpy_index], end - sg_index);
		} else {
			sg_copy_from_buffer(current_sg, nents,
					    &dest[cpy_index], end - sg_index);
		}
	}
}

static int buffer_mgr_build_mlli(struct device *dev,
				 struct sg_data_array *sg_data,
				 struct mlli_params *mlli_params)
{
	struct scatterlist *cur_sg_entry;
	uint32_t *cur_mlli_entry;
	uint32_t data_len;
	uint32_t curr_nents = 0;
	uint32_t i;

	DX_LOG_DEBUG("sg_params: sg_data->num_of_sg = 0x%08X \n",
		   (uint32_t)sg_data->num_of_sg);
	/* Allocate memory from the pointed pool */
	mlli_params->mlli_virt_addr =
		dma_pool_alloc(mlli_params->curr_pool, GFP_KERNEL,
			       &mlli_params->mlli_dma_addr);
	if (unlikely(mlli_params->mlli_virt_addr == NULL)) {
		DX_LOG_ERR("mlli table dma_pool_alloc() failed\n");
		return -ENOMEM;
	}
	cur_mlli_entry = (unsigned int *)mlli_params->mlli_virt_addr;

	/* go over all SG and link it to one MLLI table */
	for(i = 0; i < sg_data->num_of_sg;i++) {

		data_len = sg_data->sg_data_len_array[i];

		for (cur_sg_entry = sg_data->sg_array[i];
		     (cur_sg_entry != NULL) && (data_len !=0);
		     cur_sg_entry = sg_next(cur_sg_entry) , cur_mlli_entry+=2) {
			uint32_t entry_len =
				((data_len > sg_dma_len(cur_sg_entry))) ?
				sg_dma_len(cur_sg_entry) : data_len;
			data_len -= entry_len;
			cur_mlli_entry[SEP_LLI_ADDR_WORD_OFFSET] =
					sg_dma_address(cur_sg_entry);
			cur_mlli_entry[SEP_LLI_SIZE_WORD_OFFSET] =
					entry_len;
			DX_LOG_DEBUG("entry[%d] : mlli addr = 0x%08lX "
				     "mlli_len =0x%08lX\n",
				   curr_nents,
				   (unsigned long)cur_mlli_entry[SEP_LLI_ADDR_WORD_OFFSET],
				   (unsigned long)cur_mlli_entry[SEP_LLI_SIZE_WORD_OFFSET]);
			curr_nents++;
		} /* for */
		/* set last bit in the current table*/
		SEP_LLI_SET(&mlli_params->mlli_virt_addr[
			SEP_LLI_ENTRY_BYTE_SIZE*(curr_nents-1)],
			LAST, sg_data->is_last[i]);
	} /* for */
	mlli_params->mlli_len =
		curr_nents * SEP_LLI_ENTRY_BYTE_SIZE;

	SEP_LLI_SET(&mlli_params->mlli_virt_addr[
		SEP_LLI_ENTRY_BYTE_SIZE*(curr_nents-1)],
		LAST, 1);

	DX_LOG_DEBUG("MLLI params: virt addr = 0x%08lX "
		     "dma_address=0x%08lX, mlli_len =0x%08X\n",
		   (unsigned long)mlli_params->mlli_virt_addr,
		   (unsigned long)mlli_params->mlli_dma_addr,
		   (unsigned)mlli_params->mlli_len);
	return 0;
}

static inline void buffer_mgr_set_sg_entry(struct sg_data_array *sg_data,
					   unsigned int nents,
					   struct scatterlist * sg,
					   unsigned int data_len,
					   bool is_last)
{
	unsigned int index  = sg_data->num_of_sg;
	DX_LOG_DEBUG("index = %d nents %d sg 0X%X data_len 0X%X \n",
		     index, nents, (unsigned int)sg, data_len);
	sg_data->nents_array[index] = nents;
	sg_data->sg_array[index] = sg;
	sg_data->sg_data_len_array[index] = data_len;
	sg_data->is_last[index] = is_last;
	sg_data->num_of_sg++;
}

static int dx_map_sg(struct device *dev, struct scatterlist *sg,
		     unsigned int nbytes, int direction,
		     uint32_t *nents, uint32_t max_sg_nents,
		     int *lbytes)
{
	if (sg_is_last(sg)) {
		/* One entry only case -set to DLLI */
		if ( unlikely( dma_map_sg(dev, sg, 1, direction) != 1 ) ) {
			DX_LOG_ERR("dma_map_sg() single buffer failed %s\n ",
				   get_dir_type(direction));
			return -ENOMEM;
		}
		DX_LOG_DEBUG("Mapped sg: dma_address=0x%08lX "
			     "page_link=0x%08lX addr=0x%08lX offset=%u "
			     "length=%u\n",
			     (unsigned long)sg_dma_address(sg),
			     sg->page_link,
			     (unsigned long)sg_virt(sg),
			     sg->offset, sg->length);
		*lbytes = nbytes;
		*nents = 1;
	} else {  /*sg_is_last*/

		*nents = sg_count_ents(sg, nbytes, lbytes);
		if (*nents > max_sg_nents) {
			DX_LOG_ERR("Too many fragments. current %d max %d\n",
				   *nents, max_sg_nents);
			return -ENOMEM;
		}
		/* TODO - verify num of entries */
		if ( unlikely( dma_map_sg(dev, sg, *nents, direction)
			      != *nents ) ) {
			DX_LOG_ERR("dma_map_sg() sg buffer failed - %s\n",
				   get_dir_type(direction));
			return -ENOMEM;
		}
	}

	return 0;
}

static inline int dx_aead_handle_config_buf(struct device *dev,
					   struct aead_req_ctx *areq_ctx,
					   uint8_t* config_data,
					   struct sg_data_array *sg_data,
						unsigned int assoclen)
{
	DX_LOG_DEBUG(" handle additional data config set to   DLLI \n");
	/* create sg for the current buffer */
	sg_init_one(&areq_ctx->ccm_adata_sg, config_data, AES_BLOCK_SIZE + areq_ctx->ccm_hdr_size);
	if (unlikely(dma_map_sg(dev, &areq_ctx->ccm_adata_sg,
			1, DMA_TO_DEVICE) != 1)) {
			DX_LOG_ERR("dma_map_sg() "
			   "config buffer failed\n");
			return -ENOMEM;
	}
	DX_LOG_DEBUG("Mapped curr_buff: dma_address=0x%08lX "
		     "page_link=0x%08lX addr=0x%08lX "
		     "offset=%u length=%u\n",
		     (unsigned long)sg_dma_address(&areq_ctx->ccm_adata_sg),
		     areq_ctx->ccm_adata_sg.page_link,
		     (unsigned long)sg_virt(&areq_ctx->ccm_adata_sg),
		     areq_ctx->ccm_adata_sg.offset,
		     areq_ctx->ccm_adata_sg.length);
	/* prepare for case of MLLI */
	if (assoclen > 0) {
		buffer_mgr_set_sg_entry(sg_data, 1, &areq_ctx->ccm_adata_sg,
					AES_BLOCK_SIZE + areq_ctx->ccm_hdr_size, false);
	}
	return 0;
}


static inline int dx_ahash_handle_curr_buf(struct device *dev,
					   struct ahash_req_ctx *areq_ctx,
					   uint8_t* curr_buff,
					   uint32_t curr_buff_cnt,
					   struct sg_data_array *sg_data)
{
	DX_LOG_DEBUG(" handle curr buff %x set to   DLLI \n", curr_buff_cnt);
	/* create sg for the current buffer */
	sg_init_one(areq_ctx->buff_sg,curr_buff, curr_buff_cnt);
	if (unlikely(dma_map_sg(dev, areq_ctx->buff_sg,
			1, DMA_TO_DEVICE) != 1)) {
			DX_LOG_ERR("dma_map_sg() "
			   "src buffer failed\n");
			return -ENOMEM;
	}
	DX_LOG_DEBUG("Mapped curr_buff: dma_address=0x%08lX "
		     "page_link=0x%08lX addr=0x%08lX "
		     "offset=%u length=%u\n",
		     (unsigned long)sg_dma_address(areq_ctx->buff_sg),
		     areq_ctx->buff_sg->page_link,
		     (unsigned long)sg_virt(areq_ctx->buff_sg),
		     areq_ctx->buff_sg->offset,
		     areq_ctx->buff_sg->length);
	areq_ctx->data_dma_buf_type = DX_DMA_BUF_DLLI;
	areq_ctx->curr_sg = areq_ctx->buff_sg;
	areq_ctx->in_nents = 0;
	/* prepare for case of MLLI */
	buffer_mgr_set_sg_entry(sg_data, 1, areq_ctx->buff_sg,
				curr_buff_cnt, false);
	return 0;
}

static inline int dx_aead_handle_frag_icv(struct device *dev,
				    struct aead_request *req, int lbytes)
{
	struct aead_req_ctx *areq_ctx = aead_request_ctx(req);

	if ((areq_ctx->gen_ctx.op_type == SEP_CRYPTO_DIRECTION_DECRYPT) ||
	    (req->src == req->dst)) {
		areq_ctx->frag_icv_sg = req->src;
		areq_ctx->frag_icv_sg_nents = areq_ctx->in_nents;
		/*ICV only fragments count, can be up to 2 fragments.
		  More fragments return error -EINVAL */
		if ((sg_dma_len(&req->src[areq_ctx->in_nents - 2]) <
				(areq_ctx->req_authsize - lbytes))) {
			goto err;
		} else if ((sg_dma_len(&req->src[areq_ctx->in_nents - 2]) ==
				(areq_ctx->req_authsize - lbytes))) {
			areq_ctx->icv_only_frag = 2;
		} else {
			areq_ctx->icv_only_frag = 1;
		}

	} else {
		areq_ctx->frag_icv_sg = req->dst;
		areq_ctx->frag_icv_sg_nents = areq_ctx->out_nents;
	}
	areq_ctx->last_bytes = lbytes;
	areq_ctx->is_icv_frag = true;
	return 0;
err:
	return -EINVAL;
}

int map_ablkcipher_request(struct device *dev, struct ablkcipher_request *req)
{
	struct ablkcipher_req_ctx *areq_ctx = ablkcipher_request_ctx(req);
	unsigned int iv_size = crypto_ablkcipher_ivsize(crypto_ablkcipher_reqtfm(req));
	struct mlli_params *mlli_params = &areq_ctx->mlli_params;
	struct sg_data_array sg_data;
	struct buff_mgr_handle *buff_mgr = crypto_drvdata->buff_mgr_handle;
	int dummy = 0;
	int rc = 0;

	areq_ctx->sec_dir = 0;
	areq_ctx->dma_buf_type = DX_DMA_BUF_DLLI;
	mlli_params->curr_pool = NULL;
	sg_data.num_of_sg = 0;

	/* Map IV buffer */
	if (likely(iv_size != 0) ) {
		dump_byte_array("iv", (uint8_t *)req->info, iv_size);
		areq_ctx->gen_ctx.iv_dma_addr =
			dma_map_single(dev, (void *)req->info,
				       iv_size, DMA_TO_DEVICE);
		if (unlikely(dma_mapping_error(dev,
					areq_ctx->gen_ctx.iv_dma_addr))) {
			DX_LOG_ERR("Mapping iv %u B at va=0x%08lX "
				   "for DMA failed\n",iv_size,
				    (unsigned long)req->info);
			return -ENOMEM;
		}
		DX_LOG_DEBUG("Mapped iv %u B at va=0x%08lX to dma=0x%08lX\n",
				iv_size, (unsigned long)req->info,
			     (unsigned long)areq_ctx->gen_ctx.iv_dma_addr);
	} else {
		areq_ctx->gen_ctx.iv_dma_addr = 0;
	}

	/* Map the src sg */
	if ( sg_is_last(req->src) &&
	     (sg_page(req->src) == NULL) &&
	     sg_dma_address(req->src)) {
		/* The source is secure no mapping is needed */
		areq_ctx->sec_dir = DX_SRC_DMA_IS_SECURE;
		areq_ctx->in_nents = 1;
	} else {
		if ( unlikely( dx_map_sg( dev,req->src, req->nbytes,
					  DMA_BIDIRECTIONAL,
					  &areq_ctx->in_nents,
					  LLI_MAX_NUM_OF_DATA_ENTRIES,
					  &dummy))){
			rc = -ENOMEM;
			goto fail_unmap_iv;
		}

		if ( areq_ctx->in_nents > 1 ) {
			areq_ctx->dma_buf_type = DX_DMA_BUF_MLLI;
		}
	}

	if ( unlikely(req->src == req->dst)) {
		if ( areq_ctx->sec_dir == DX_SRC_DMA_IS_SECURE ) {
			DX_LOG_ERR("Secure key inplace operation "
				   "is not supported \n");
			/* both sides are secure */
			rc = -ENOMEM;
			goto fail_unmap_din;
		}
		/* Handle inplace operation */
		if ( unlikely(areq_ctx->dma_buf_type == DX_DMA_BUF_MLLI) ) {
			areq_ctx->out_nents = 0;
			buffer_mgr_set_sg_entry(&sg_data,
						areq_ctx->in_nents,
						req->src,
						req->nbytes,
						true);
		}
	} else {
		if ( sg_is_last(req->dst) &&
		     (sg_page(req->dst) == NULL) &&
		     sg_dma_address(req->dst)) {
			if ( areq_ctx->sec_dir == DX_SRC_DMA_IS_SECURE ) {
				DX_LOG_ERR("Secure key in both sides is"
					   "not supported \n");
				/* both sides are secure */
				rc = -ENOMEM;
				goto fail_unmap_din;
			}
			/* The dest is secure no mapping is needed */
			areq_ctx->sec_dir = DX_DST_DMA_IS_SECURE;
			areq_ctx->out_nents = 1;
		} else {
			/* Map the dst sg */
			if ( unlikely( dx_map_sg(dev,req->dst, req->nbytes,
						 DMA_BIDIRECTIONAL,
						 &areq_ctx->out_nents,
						 LLI_MAX_NUM_OF_DATA_ENTRIES,
						 &dummy))){
				rc = -ENOMEM;
				goto fail_unmap_din;
			}

			if ( areq_ctx->out_nents > 1 ) {
				areq_ctx->dma_buf_type = DX_DMA_BUF_MLLI;
			}
		}
		if ( unlikely( (areq_ctx->dma_buf_type == DX_DMA_BUF_MLLI) ) ) {
			if (areq_ctx->sec_dir != DX_SRC_DMA_IS_SECURE) {
				buffer_mgr_set_sg_entry(&sg_data,
							areq_ctx->in_nents,
							req->src,
							req->nbytes,
							true);
			}
			if (areq_ctx->sec_dir != DX_DST_DMA_IS_SECURE) {
				buffer_mgr_set_sg_entry(&sg_data,
							areq_ctx->out_nents,
							req->dst,
							req->nbytes,
							true);
			}
		} /*few entries */
	} /* !inplace */

	if (unlikely(areq_ctx->dma_buf_type == DX_DMA_BUF_MLLI)) {
#if (DX_DEV_SIGNATURE == DX_CC441P_SIG)
		if (areq_ctx->sec_dir) {
			/* one of the sides is secure, can't use MLLI*/
			rc = -EINVAL;
			goto fail_unmap_dout;
		}
#endif
		mlli_params->curr_pool = buff_mgr->mlli_buffs_pool;
		if (unlikely(buffer_mgr_build_mlli(dev, &sg_data, mlli_params))) {
			rc = -ENOMEM;
			goto fail_unmap_dout;
		}
	} /*MLLI case*/

	DX_LOG_DEBUG(" buf type = %s \n",
		     dx_get_buff_type(areq_ctx->dma_buf_type));
	return 0;
fail_unmap_dout:
	if (areq_ctx->sec_dir != DX_DST_DMA_IS_SECURE) {
		dma_unmap_sg(dev, req->dst,
			     areq_ctx->out_nents, DMA_BIDIRECTIONAL);
	}
fail_unmap_din:
	if (areq_ctx->sec_dir != DX_SRC_DMA_IS_SECURE) {
		dma_unmap_sg(dev, req->src,
			     areq_ctx->in_nents, DMA_BIDIRECTIONAL);
	}
fail_unmap_iv:
	if (areq_ctx->gen_ctx.iv_dma_addr != 0) {
		dma_unmap_single(dev, areq_ctx->gen_ctx.iv_dma_addr,
				 iv_size, DMA_TO_DEVICE);
	}
	return rc;
}

void unmap_ablkcipher_request(struct device *dev,
			      struct ablkcipher_request *req)
{
	struct ablkcipher_req_ctx *areq_ctx;
	unsigned int iv_size;

	areq_ctx = ablkcipher_request_ctx(req);
	iv_size = crypto_ablkcipher_ivsize(crypto_ablkcipher_reqtfm(req));

	if (likely(areq_ctx->gen_ctx.iv_dma_addr != 0)) {
		DX_LOG_DEBUG("Unmapped iv: iv_dma_addr=0x%08lX iv_size=%d\n",
			     (unsigned long)areq_ctx->gen_ctx.iv_dma_addr, iv_size);
		dma_unmap_single(dev, areq_ctx->gen_ctx.iv_dma_addr,
				 iv_size, DMA_TO_DEVICE);
	}
	/*In case a pool was set, a table was allocated and should be released */
	if (areq_ctx->dma_buf_type == DX_DMA_BUF_MLLI) {
		dma_pool_free(areq_ctx->mlli_params.curr_pool,
			      areq_ctx->mlli_params.mlli_virt_addr,
			      areq_ctx->mlli_params.mlli_dma_addr);
	}

	if (areq_ctx->sec_dir != DX_SRC_DMA_IS_SECURE) {
		dma_unmap_sg(dev, req->src,
			     areq_ctx->in_nents, DMA_BIDIRECTIONAL);
	}
	DX_LOG_DEBUG("Unmapped sg src: req->src=0x%08lX\n",
		      (unsigned long)sg_virt(req->src));
	if (likely(req->src != req->dst)) {
		if (areq_ctx->sec_dir != DX_DST_DMA_IS_SECURE) {
			dma_unmap_sg(dev, req->dst,
				     areq_ctx->out_nents, DMA_BIDIRECTIONAL);
			DX_LOG_DEBUG("Unmapped sg dst: req->dst=0x%08lX\n",
				     (unsigned long)sg_virt(req->dst));
		}
	}
}

void unmap_aead_request(struct device *dev, struct aead_request *req)
{
	struct aead_req_ctx *areq_ctx = aead_request_ctx(req);
	unsigned int hw_iv_size = areq_ctx->hw_iv_size;

/*HI3630++ DX: for SCCC bug*/
//HI3630	if (!areq_ctx->mac_buf_dma_addr)
	if (areq_ctx->mac_buf_dma_addr != 0)
/*HI3630--*/
		dma_unmap_single(dev, areq_ctx->mac_buf_dma_addr,
			MAX_MAC_SIZE, DMA_BIDIRECTIONAL);

	if (areq_ctx->ccm_hdr_size != ccm_header_size_null) {
/*HI3630++ DX: for SCCC bug*/
//HI3630	if (!areq_ctx->ccm_iv0_dma_addr)
		if (areq_ctx->ccm_iv0_dma_addr != 0)
/*HI3630--*/
			dma_unmap_single(dev, areq_ctx->ccm_iv0_dma_addr,
				AES_BLOCK_SIZE, DMA_TO_DEVICE);
		if (&areq_ctx->ccm_adata_sg != NULL)
			dma_unmap_sg(dev, &areq_ctx->ccm_adata_sg,
				1, DMA_TO_DEVICE);
	}
/*HI3630++ DX: for SCCC bug*/
//HI3630	if (!areq_ctx->gen_ctx.iv_dma_addr)
	if (areq_ctx->gen_ctx.iv_dma_addr != 0)
/*HI3630--*/
		dma_unmap_single(dev, areq_ctx->gen_ctx.iv_dma_addr,
				 hw_iv_size, DMA_BIDIRECTIONAL);

	/*In case a pool was set, a table was
	  allocated and should be released */
	if (areq_ctx->mlli_params.curr_pool != NULL) {
		DX_LOG_DEBUG("free MLLI buffer: dma=0x%08lX virt=0x%08X\n",
			     (unsigned long)areq_ctx->mlli_params.mlli_dma_addr,
			     (uint32_t)areq_ctx->mlli_params.mlli_virt_addr);
		dma_pool_free(areq_ctx->mlli_params.curr_pool,
			      areq_ctx->mlli_params.mlli_virt_addr,
			      areq_ctx->mlli_params.mlli_dma_addr);
	}
	if (areq_ctx->assoc_dma_buf_type != DX_DMA_BUF_NULL) {
		DX_LOG_DEBUG("Unmapping sg assoc: req->assoc=0x%08lX\n",
			     (unsigned long)sg_virt(req->assoc));
		dma_unmap_sg(dev, req->assoc,  areq_ctx->assoc_nents,
			DMA_TO_DEVICE);
	}

	DX_LOG_DEBUG("Unmapping sg src: req->src=0x%08lX\n",
		     (unsigned long)sg_virt(req->src));
	dma_unmap_sg(dev, req->src,
		     areq_ctx->in_nents, DMA_BIDIRECTIONAL);
	if (unlikely(req->src != req->dst)) {
		DX_LOG_DEBUG("Unmapping sg dst: req->dst=0x%08lX\n",
			     (unsigned long)sg_virt(req->dst));
		dma_unmap_sg(dev, req->dst,
			     areq_ctx->out_nents, DMA_BIDIRECTIONAL);
	}
}

int map_aead_request(struct device *dev, struct aead_request *req)
{
	struct aead_req_ctx *areq_ctx = aead_request_ctx(req);
	unsigned int hw_iv_size = areq_ctx->hw_iv_size;
	struct mlli_params *mlli_params = &areq_ctx->mlli_params;
	struct sg_data_array sg_data;
	unsigned int authsize = areq_ctx->req_authsize;
	unsigned int size_for_map;
	struct buff_mgr_handle *buff_mgr = crypto_drvdata->buff_mgr_handle;
	int dummy = 0; /*used for the assoc data fragments */
	int src_last_bytes = 0;
	int dst_last_bytes = 0;
	int rc = -ENOMEM;

	areq_ctx->assoc_dma_buf_type = DX_DMA_BUF_NULL;
	areq_ctx->data_dma_buf_type = DX_DMA_BUF_DLLI;
	mlli_params->curr_pool = NULL;
	areq_ctx->out_nents = 0;
	areq_ctx->is_icv_frag = false;
	sg_data.num_of_sg = 0;

	/* cacluate the size for cipher remove ICV in decrypt*/
	areq_ctx->cryptlen = (areq_ctx->gen_ctx.op_type ==
				 SEP_CRYPTO_DIRECTION_ENCRYPT) ?
				req->cryptlen :
				(req->cryptlen - authsize);

	areq_ctx->mac_buf_dma_addr = dma_map_single(dev,
		areq_ctx->mac_buf, MAX_MAC_SIZE, DMA_BIDIRECTIONAL);
	if (unlikely(dma_mapping_error(dev, areq_ctx->mac_buf_dma_addr))) {
		DX_LOG_ERR("Mapping mac_buf %u B at va=0x%08lX "
			"for DMA failed\n", MAX_MAC_SIZE,
			(unsigned long)areq_ctx->mac_buf);
		goto aead_map_failure;
	}

	if (likely(req->iv)) {
		dump_byte_array("iv", (uint8_t *)req->iv, hw_iv_size);
		areq_ctx->gen_ctx.iv_dma_addr = dma_map_single(dev,
			req->iv, hw_iv_size, DMA_BIDIRECTIONAL);
		if (unlikely(dma_mapping_error(
			dev, areq_ctx->gen_ctx.iv_dma_addr))) {
			DX_LOG_ERR("Mapping iv %u B at va=0x%08lX "
				   "for DMA failed\n",hw_iv_size,
				     (unsigned long)req->iv);
			goto aead_map_failure;
		}
		DX_LOG_DEBUG("Mapped iv %u B at va=0x%08lX to dma=0x%08lX\n",
			hw_iv_size, (unsigned long)req->iv,
			(unsigned long)areq_ctx->gen_ctx.iv_dma_addr);
	} else {
		areq_ctx->gen_ctx.iv_dma_addr = 0;
	}

	if (areq_ctx->ccm_hdr_size != ccm_header_size_null) {
		areq_ctx->ccm_iv0_dma_addr = dma_map_single(dev,
			(areq_ctx->ccm_config + CCM_CTR_COUNT_0_OFFSET),
			AES_BLOCK_SIZE, DMA_TO_DEVICE);

		if (unlikely(dma_mapping_error(dev, areq_ctx->ccm_iv0_dma_addr))) {
			DX_LOG_ERR("Mapping mac_buf %u B at va=0x%08lX "
			"for DMA failed\n", AES_BLOCK_SIZE, (unsigned long)
			(areq_ctx->ccm_config + CCM_CTR_COUNT_0_OFFSET));
			areq_ctx->ccm_iv0_dma_addr = 0;
			goto aead_map_failure;
		}
		if (dx_aead_handle_config_buf(dev, areq_ctx,
					      areq_ctx->ccm_config,
					      &sg_data,req->assoclen)){
			goto aead_map_failure;
		}
	}

	/*associated data*/
	if (req->assoclen != 0 ) {
		if (unlikely( dx_map_sg(dev, req->assoc,
					 req->assoclen,
					 DMA_TO_DEVICE,
					 &areq_ctx->assoc_nents,
					 LLI_MAX_NUM_OF_ASSOC_DATA_ENTRIES,
					 &dummy))) {
			goto aead_map_failure;
		}
		/*in CCM case we have additional entry for ccm header configurations */
		if (areq_ctx->ccm_hdr_size != ccm_header_size_null) {
			if (unlikely((areq_ctx->assoc_nents + 1) >
				LLI_MAX_NUM_OF_ASSOC_DATA_ENTRIES)) {
				DX_LOG_ERR("CCM case.Too many fragments. "
					   "Current %d max %d\n",
				(areq_ctx->assoc_nents + 1),
				LLI_MAX_NUM_OF_ASSOC_DATA_ENTRIES);
				goto aead_map_failure;
			}
		}

		if (likely(areq_ctx->assoc_nents == 1) &&
		    (areq_ctx->ccm_hdr_size == ccm_header_size_null)) {
			areq_ctx->assoc_dma_buf_type = DX_DMA_BUF_DLLI;
		} else {
			areq_ctx->assoc_dma_buf_type = DX_DMA_BUF_MLLI;
			buffer_mgr_set_sg_entry(&sg_data,
						areq_ctx->assoc_nents,
						req->assoc,
						req->assoclen,
						true);
			DX_LOG_DEBUG("Non-contig. assoc: buf type = %s "
				"assoc_nents 0x%08lX \n",
				dx_get_buff_type(areq_ctx->assoc_dma_buf_type),
				(long unsigned int)(areq_ctx->assoc_nents));
		}
	} else {
		areq_ctx->assoc_dma_buf_type = DX_DMA_BUF_NULL;
	}

	if (likely(req->src == req->dst)) {
		size_for_map = (areq_ctx->gen_ctx.op_type ==
				SEP_CRYPTO_DIRECTION_ENCRYPT) ?
				(req->cryptlen + authsize) :
				(req->cryptlen);
	} else {
		/* in case of non-inplace the
		source len is alway cryptlen */
		size_for_map = req->cryptlen;
	}

	if (unlikely(dx_map_sg(dev, req->src, size_for_map,
		DMA_BIDIRECTIONAL, &areq_ctx->in_nents,
		LLI_MAX_NUM_OF_DATA_ENTRIES, &src_last_bytes)))
		goto aead_map_failure;

	if (areq_ctx->in_nents > 1)
		areq_ctx->data_dma_buf_type = DX_DMA_BUF_MLLI;

	if (likely(req->src == req->dst)) {
		if (unlikely(areq_ctx->data_dma_buf_type == DX_DMA_BUF_MLLI)) {
			areq_ctx->out_nents = 0;
			buffer_mgr_set_sg_entry(&sg_data,
						areq_ctx->in_nents,
						req->src,
						areq_ctx->cryptlen,
						true);
		}
	} else {
		/* non inplace operation,
		calculate the size for dst buffer */
		size_for_map = (areq_ctx->gen_ctx.op_type ==
				SEP_CRYPTO_DIRECTION_ENCRYPT) ?
				(req->cryptlen + authsize) :
				(req->cryptlen - authsize);
		if (unlikely(dx_map_sg(dev, req->dst, size_for_map,
					 DMA_BIDIRECTIONAL,
					 &areq_ctx->out_nents,
					 LLI_MAX_NUM_OF_DATA_ENTRIES,
					 &dst_last_bytes))) {
			goto aead_map_failure;
		}

		if (areq_ctx->out_nents > 1)
			areq_ctx->data_dma_buf_type = DX_DMA_BUF_MLLI;

		if (unlikely(areq_ctx->data_dma_buf_type == DX_DMA_BUF_MLLI)) {
			buffer_mgr_set_sg_entry(&sg_data,
						areq_ctx->in_nents,
						req->src,
						areq_ctx->cryptlen,
						true);
			buffer_mgr_set_sg_entry(&sg_data,
						areq_ctx->out_nents,
						req->dst,
						areq_ctx->cryptlen,
						true);
		}
	}

	/*ICV handling */
	if ((areq_ctx->out_nents == 0) ||
	    (areq_ctx->gen_ctx.op_type == SEP_CRYPTO_DIRECTION_DECRYPT))  {
		if (likely(src_last_bytes >= authsize)) {
			if (likely(src_last_bytes != authsize))
				areq_ctx->icv_only_frag = 0;
			else
				/*The ICV is not fragmented but place
				in the last sg enrtry without data */
				areq_ctx->icv_only_frag = 1;

			areq_ctx->icv_sg = &req->src[areq_ctx->in_nents - 1];
			areq_ctx->icv_offset = src_last_bytes - authsize;
		} else {
			rc = dx_aead_handle_frag_icv(dev, req, src_last_bytes);
			if (rc != 0)
				goto aead_map_failure;
		}
	} else {
		areq_ctx->icv_only_frag = 0;
		if (likely(dst_last_bytes >= authsize)) {
			areq_ctx->icv_sg = &req->dst[areq_ctx->out_nents - 1];
			areq_ctx->icv_offset = dst_last_bytes - authsize;
		} else {
			rc = dx_aead_handle_frag_icv(dev, req, dst_last_bytes);
			if (rc != 0)
				goto aead_map_failure;
		}
	}

	/* Mlli support - start building the MLLI according to
	   the above results*/
	if ((unlikely(areq_ctx->data_dma_buf_type == DX_DMA_BUF_MLLI) ||
	    (areq_ctx->assoc_dma_buf_type == DX_DMA_BUF_MLLI))) {
		mlli_params->curr_pool = buff_mgr->mlli_buffs_pool;
		/*only if one of the pools was set there is MLLI operation*/
		if (unlikely(buffer_mgr_build_mlli(dev, &sg_data,
						  mlli_params))) {
			goto aead_map_failure;
		}
	}

	return 0;

aead_map_failure:
	unmap_aead_request(dev, req);
	return rc;
}

int map_ahash_request_final(struct device *dev, struct ahash_request *req, bool do_update)
{
	struct ahash_req_ctx *areq_ctx = ahash_request_ctx(req);
	uint8_t* curr_buff = areq_ctx->buff_index ? areq_ctx->buff1 :
			areq_ctx->buff0;
	uint32_t *curr_buff_cnt = areq_ctx->buff_index ? &areq_ctx->buff1_cnt :
			&areq_ctx->buff0_cnt;
	struct mlli_params *mlli_params = &areq_ctx->mlli_params;
	struct sg_data_array sg_data;
	struct buff_mgr_handle *buff_mgr = crypto_drvdata->buff_mgr_handle;
	int dummy = 0;

	DX_LOG_DEBUG(" final params : curr_buff =0x%X "
		     "curr_buff_cnt=0x%X req->nbytes = 0x%X "
		     "req->src = 0x%X curr_index %d\n",
		     (uint32_t)curr_buff, *curr_buff_cnt, req->nbytes,
		     (uint32_t)req->src, areq_ctx->buff_index);
	/* Init the type of the dma buffer */
	areq_ctx->data_dma_buf_type = DX_DMA_BUF_NULL;
	mlli_params->curr_pool = NULL;
	sg_data.num_of_sg = 0;
	areq_ctx->in_nents = 0;

	if (unlikely(req->nbytes == 0 && *curr_buff_cnt == 0)) {
		/* nothing to do */
		return 0;
	}

	/*TODO: copy data in case that buffer is enough for operation */
	/* map the previous buffer */
	if (*curr_buff_cnt != 0 ) {
		if (dx_ahash_handle_curr_buf(dev, areq_ctx, curr_buff,
					    *curr_buff_cnt, &sg_data)){
			return -ENOMEM;
		}
	}

	if ((req->nbytes) && do_update) {
		if ( unlikely( dx_map_sg( dev,req->src,
					  req->nbytes,
					  DMA_TO_DEVICE,
					  &areq_ctx->in_nents,
					  LLI_MAX_NUM_OF_DATA_ENTRIES,
					  &dummy))){
			goto unmap_curr_buff;
		}
		if ( (areq_ctx->in_nents == 1)
		     && (areq_ctx->data_dma_buf_type == DX_DMA_BUF_NULL) ) {
			memcpy(areq_ctx->buff_sg,req->src,
			       sizeof(struct scatterlist));
			areq_ctx->buff_sg->length = req->nbytes;
			areq_ctx->curr_sg = areq_ctx->buff_sg;
			areq_ctx->data_dma_buf_type = DX_DMA_BUF_DLLI;
		} else {
			areq_ctx->data_dma_buf_type = DX_DMA_BUF_MLLI;
		}

	}

	/*build mlli */
	if (unlikely(areq_ctx->data_dma_buf_type == DX_DMA_BUF_MLLI)) {
		mlli_params->curr_pool = buff_mgr->mlli_buffs_pool;
		/* add the src data to the sg_data */
		buffer_mgr_set_sg_entry(&sg_data,
					areq_ctx->in_nents,
					req->src,
					req->nbytes,
					true);
		if (unlikely(buffer_mgr_build_mlli(dev, &sg_data,
						  mlli_params))) {
			goto fail_unmap_din;
		}
	}
	/* change the buffer index for the unmap function */
	areq_ctx->buff_index = (areq_ctx->buff_index^1);
	DX_LOG_DEBUG(" buf type = %s \n\n",
		     dx_get_buff_type(areq_ctx->data_dma_buf_type));
	return 0;

fail_unmap_din:
	dma_unmap_sg(dev, req->src, areq_ctx->in_nents, DMA_TO_DEVICE);

unmap_curr_buff:
	if (*curr_buff_cnt != 0 ) {
		dma_unmap_sg(dev, areq_ctx->buff_sg, 1, DMA_TO_DEVICE);
	}
	return -ENOMEM;
}


int map_ahash_request_update(struct device *dev, struct ahash_request *req)
{
	struct crypto_ahash *ahash = crypto_ahash_reqtfm(req);
	struct ahash_req_ctx *areq_ctx = ahash_request_ctx(req);
	uint8_t* curr_buff = areq_ctx->buff_index ? areq_ctx->buff1 :
			areq_ctx->buff0;
	uint32_t *curr_buff_cnt = areq_ctx->buff_index ? &areq_ctx->buff1_cnt :
			&areq_ctx->buff0_cnt;
	uint8_t* next_buff = areq_ctx->buff_index ? areq_ctx->buff0 :
			areq_ctx->buff1;
	uint32_t *next_buff_cnt = areq_ctx->buff_index ? &areq_ctx->buff0_cnt :
			&areq_ctx->buff1_cnt;
	unsigned int block_size = crypto_tfm_alg_blocksize(&ahash->base);
	struct mlli_params *mlli_params = &areq_ctx->mlli_params;
	unsigned int update_data_len;
	int total_in_len = req->nbytes + *curr_buff_cnt;
	struct sg_data_array sg_data;
	struct buff_mgr_handle *buff_mgr = crypto_drvdata->buff_mgr_handle;
	unsigned int swap_index = 0;
	int dummy = 0;

	DX_LOG_DEBUG(" update params : curr_buff =0x%X "
		     "curr_buff_cnt=0x%X req->nbytes = 0x%X "
		     "req->src = 0x%X curr_index = %d \n",
		     (uint32_t)curr_buff, *curr_buff_cnt, req->nbytes,
		     (uint32_t)req->src, areq_ctx->buff_index);
	/* Init the type of the dma buffer */
	areq_ctx->data_dma_buf_type = DX_DMA_BUF_NULL;
	mlli_params->curr_pool = NULL;
	areq_ctx->curr_sg = NULL;
	sg_data.num_of_sg = 0;
	areq_ctx->in_nents = 0;

	if (unlikely(total_in_len < block_size)) {
		DX_LOG_DEBUG(" less than one block: curr_buff =0x%X "
			     "*curr_buff_cnt =0x%x copy_to =0x%X\n",
			     (uint32_t)curr_buff, (uint32_t)*curr_buff_cnt,
			   (uint32_t)&curr_buff[*curr_buff_cnt]);
		areq_ctx->in_nents = sg_count_ents(req->src,req->nbytes, &dummy);
		sg_copy_to_buffer(req->src, areq_ctx->in_nents,
				  &curr_buff[*curr_buff_cnt], req->nbytes);
		*curr_buff_cnt += req->nbytes;
		return 1;
	}

	/* Calculate the residue size*/
	*next_buff_cnt = total_in_len & (block_size - 1);
	/* update data len */
	update_data_len = total_in_len - *next_buff_cnt;

	DX_LOG_DEBUG(" temp length : *next_buff_cnt =0x%X "
		     "update_data_len=0x%X\n",
		     (uint32_t)*next_buff_cnt, update_data_len);

	/* Copy the new residue to next buffer */
	if (*next_buff_cnt != 0) {
		DX_LOG_DEBUG(" handle residue: next buff %x skip data %x"
			     " residue %x \n",(unsigned int)next_buff,
			     (update_data_len -*curr_buff_cnt),
			     *next_buff_cnt);
		dx_sg_copy_part(next_buff, req->src,
			     (update_data_len -*curr_buff_cnt),
			     req->nbytes,DX_SG_TO_BUF);
		/* change the buffer index for next operation */
		swap_index = 1;
	}

	if (*curr_buff_cnt != 0) {
		if (dx_ahash_handle_curr_buf(dev, areq_ctx, curr_buff,
					    *curr_buff_cnt, &sg_data)){
			return -ENOMEM;
		}
		/* change the buffer index for next operation */
		swap_index = 1;
	}

	if ( update_data_len > *curr_buff_cnt ) {
		if ( unlikely( dx_map_sg( dev,req->src,
					  (update_data_len -*curr_buff_cnt),
					  DMA_TO_DEVICE,
					  &areq_ctx->in_nents,
					  LLI_MAX_NUM_OF_DATA_ENTRIES,
					  &dummy))){
			goto unmap_curr_buff;
		}
		if ( (areq_ctx->in_nents == 1)
		     && (areq_ctx->data_dma_buf_type == DX_DMA_BUF_NULL) ) {
			/* only one entry in the SG and no previous data */
			memcpy(areq_ctx->buff_sg,req->src,
			       sizeof(struct scatterlist));
			areq_ctx->buff_sg->length = update_data_len;
			areq_ctx->data_dma_buf_type = DX_DMA_BUF_DLLI;
			areq_ctx->curr_sg = areq_ctx->buff_sg;
		} else {
			areq_ctx->data_dma_buf_type = DX_DMA_BUF_MLLI;
		}
	}

	if (unlikely(areq_ctx->data_dma_buf_type == DX_DMA_BUF_MLLI)) {
		mlli_params->curr_pool = buff_mgr->mlli_buffs_pool;
		/* add the src data to the sg_data */
		buffer_mgr_set_sg_entry(&sg_data,
					areq_ctx->in_nents,
					req->src,
					(update_data_len -*curr_buff_cnt),
					true);
		if (unlikely(buffer_mgr_build_mlli(dev, &sg_data,
						  mlli_params))) {
			goto fail_unmap_din;
		}

	}
	areq_ctx->buff_index = (areq_ctx->buff_index^swap_index);

	DX_LOG_DEBUG(" buf type = %s \n\n",
		     dx_get_buff_type(areq_ctx->data_dma_buf_type));
	return 0;


fail_unmap_din:
	dma_unmap_sg(dev, req->src, areq_ctx->in_nents, DMA_TO_DEVICE);

unmap_curr_buff:
	if (*curr_buff_cnt != 0 ) {
		dma_unmap_sg(dev, areq_ctx->buff_sg, 1, DMA_TO_DEVICE);
	}
	return -ENOMEM;
}

void unmap_ahash_request(struct device *dev, struct ahash_request *req, bool do_revert)
{
	struct ahash_req_ctx *areq_ctx = ahash_request_ctx(req);
	uint32_t *prev_len = areq_ctx->buff_index ?  &areq_ctx->buff0_cnt :
						&areq_ctx->buff1_cnt;

	/*In case a pool was set, a table was
	  allocated and should be released */
	if (areq_ctx->mlli_params.curr_pool != NULL) {
		DX_LOG_DEBUG("free MLLI buffer: dma=0x%08lX virt=0x%08X\n",
			     (unsigned long)areq_ctx->mlli_params.mlli_dma_addr,
			     (uint32_t)areq_ctx->mlli_params.mlli_virt_addr);
		dma_pool_free(areq_ctx->mlli_params.curr_pool,
			      areq_ctx->mlli_params.mlli_virt_addr,
			      areq_ctx->mlli_params.mlli_dma_addr);
	}

	if (likely(areq_ctx->in_nents != 0)) {
		DX_LOG_DEBUG("Unmapped sg src: virt = 0x%08lX "
			     "dma 0x%lX len %x\n",
			     (unsigned long)sg_virt(req->src),
			     (unsigned long)sg_dma_address(req->src),
			     sg_dma_len(req->src));

		dma_unmap_sg(dev, req->src,
			     areq_ctx->in_nents, DMA_TO_DEVICE);
	}

	if (*prev_len != 0) {
		DX_LOG_DEBUG("Unmapped buffer: areq_ctx->buff_sg=0x%08lX"
			     "dma = 0x%08lX len %x\n",
				(unsigned long)sg_virt(areq_ctx->buff_sg),
				(unsigned long)sg_dma_address(areq_ctx->buff_sg),
				sg_dma_len(areq_ctx->buff_sg));
		dma_unmap_sg(dev, areq_ctx->buff_sg, 1, DMA_TO_DEVICE);
		if (!do_revert) {
			/* clean the previous data length for update operation */
			*prev_len = 0;
		} else {
			areq_ctx->buff_index ^= 1;
		}
	}
}

/*
   This function should be used by kerenl user to build specail scatrer list
   for the secure key operation the output of this function is 1 entry
   scatterlist with teh below paramters:
#ifdef CONFIG_DEBUG_SG
	unsigned long	sg_magic; - set to default in sg_init_table.
#endif
	unsigned long	page_link; - page link is NULL and the last flag is set.
	unsigned int	offset; - offset is zero
	unsigned int	length; - buffer length according to the user param.
	dma_addr_t	dma_address; - dma_addr accroding to user param.
#ifdef CONFIG_NEED_SG_DMA_LENGTH
	unsigned int	dma_length; - buffer length according to the user param.
#endif

	The specail scatterlist will be validate with the NULL page_link.
*/

int
dx_sg_init_one(struct scatterlist *sg, dma_addr_t addr, unsigned int length)
{
	if (sg == NULL) {
		return -EINVAL;
	}
	/* Init the table */
	sg_init_table(sg, 1);
	/* set the dma addr */
	sg_dma_address(sg) = addr;
	/* set the length */
	sg_dma_len(sg) = length;

	return 0;
}
EXPORT_SYMBOL(dx_sg_init_one);

int buffer_mgr_init(struct dx_drvdata *drvdata)
{

	struct buff_mgr_handle * buff_mgr_handle;
	crypto_drvdata = drvdata;

	buff_mgr_handle = (struct buff_mgr_handle *)
		kmalloc(sizeof(struct buff_mgr_handle), GFP_KERNEL);
	if (buff_mgr_handle == NULL) {
		return -ENOMEM;
	}

	buff_mgr_handle->mlli_buffs_pool = dma_pool_create(
				"dx_single_mlli_tables", drvdata->dev,
				(2 * LLI_MAX_NUM_OF_DATA_ENTRIES +
				LLI_MAX_NUM_OF_ASSOC_DATA_ENTRIES) *
				SEP_LLI_ENTRY_BYTE_SIZE,
				MLLI_TABLE_MIN_ALIGNMENT, 0);

	if (unlikely(buff_mgr_handle->mlli_buffs_pool == NULL)) {
		goto error;
	}

	drvdata->buff_mgr_handle = buff_mgr_handle;
	return 0;
error:
	buffer_mgr_fini(drvdata);
	return -ENOMEM;

}

int buffer_mgr_fini(struct dx_drvdata *drvdata)
{
	struct buff_mgr_handle * buff_mgr_handle = drvdata->buff_mgr_handle;

	if (buff_mgr_handle  != NULL) {
		dma_pool_destroy(buff_mgr_handle->mlli_buffs_pool);
		kfree(drvdata->buff_mgr_handle);
		drvdata->buff_mgr_handle = NULL;

	}
	return 0;
}
