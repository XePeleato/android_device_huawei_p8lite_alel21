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

/* \file dx_aead.h
   Discretix AEAD Crypto API
 */

#ifndef __DX_AEAD_H__
#define __DX_AEAD_H__

#include <linux/kernel.h>
#include <crypto/algapi.h>
#include <crypto/ctr.h>


/* mac_cmp - HW writes 8 B but all bytes hold the same value */
#define ICV_CMP_SIZE 8
#define CCM_CONFIG_BUF_SIZE (AES_BLOCK_SIZE*3)
#define MAX_MAC_SIZE MAX(SHA256_DIGEST_SIZE, AES_BLOCK_SIZE)


/* Offsets into AES CCM configuration buffer */
#define CCM_B0_OFFSET 0
#define CCM_A0_OFFSET 16
#define CCM_CTR_COUNT_0_OFFSET 32


enum aead_ccm_header_size {
	ccm_header_size_null = -1,
	ccm_header_size_zero = 0,
	ccm_header_size_2 = 2,
	ccm_header_size_6 = 6,
	ccm_header_size_max = INT32_MAX
};

struct aead_req_ctx {
	/* Allocate cache line although only 4 bytes are needed to
	*  assure next field falls @ cache line
	* Used for both: digest HW compare and CCM MAC value */
	uint8_t mac_buf[MAX_MAC_SIZE] ____cacheline_aligned;
	uint8_t ctr_iv[AES_BLOCK_SIZE] ____cacheline_aligned;
	uint8_t ccm_config[CCM_CONFIG_BUF_SIZE] ____cacheline_aligned;
	unsigned int hw_iv_size ____cacheline_aligned; /*HW actual size input*/
	uint8_t backup_mac[MAX_MAC_SIZE]; /*used to prevent cache coherence problem*/
	uint8_t *backup_iv; /*store iv for generated IV flow*/
	uint8_t *backup_giv; /*store iv for rfc3686(ctr) flow*/
	dma_addr_t mac_buf_dma_addr; /* internal ICV DMA buffer */
	dma_addr_t ccm_iv0_dma_addr; /* buffer for internal ccm configurations */
	struct async_gen_req_ctx gen_ctx;
	enum dx_req_dma_buf_type assoc_dma_buf_type;
	enum dx_req_dma_buf_type data_dma_buf_type;
	unsigned int assoc_nents;
	unsigned int in_nents;
	unsigned int out_nents;
	struct mlli_params mlli_params;
	unsigned int cryptlen;
	unsigned int icv_offset;
	struct scatterlist *icv_sg;
	/*fields for fragmented ICV handling */
	bool is_icv_frag;
	struct scatterlist *frag_icv_sg; /*sg to take/put the ICV*/
	int frag_icv_sg_nents; /*length of the above sg */
	int last_bytes; /*#of bytes in the last entry  */
	unsigned int icv_only_frag; /*set to one if last fragment holds only ICV */
	struct scatterlist ccm_adata_sg;
	enum aead_ccm_header_size ccm_hdr_size;
	unsigned int req_authsize;
};

int dx_aead_alloc(struct dx_drvdata *drvdata);

int dx_aead_free(struct dx_drvdata *drvdata);

#endif /*__DX_AEAD_H__*/
