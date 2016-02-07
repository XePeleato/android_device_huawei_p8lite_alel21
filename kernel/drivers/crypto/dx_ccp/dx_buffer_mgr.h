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

/* \file buffer_mgr.h
   Buffer Manager
 */

#ifndef __BUFFER_MGR_H__
#define __BUFFER_MGR_H__

#include <crypto/algapi.h>

#include "dx_config.h"
#include "dx_driver.h"

enum dx_secure_dir_type {
	DX_SRC_DMA_IS_SECURE = 1,
	DX_DST_DMA_IS_SECURE = 2
};


enum dx_req_dma_buf_type {
	DX_DMA_BUF_NULL = 0,
	DX_DMA_BUF_DLLI,
	DX_DMA_BUF_MLLI
};

enum dx_sg_cpy_direct {
	DX_SG_TO_BUF = 0,
	DX_SG_FROM_BUF = 1
};

struct mlli_params {
	struct dma_pool* curr_pool;
	uint8_t  *mlli_virt_addr;
	dma_addr_t mlli_dma_addr;
	uint32_t  mlli_len;
};




int buffer_mgr_init(struct dx_drvdata *drvdata);

int buffer_mgr_fini(struct dx_drvdata *drvdata);

int map_ablkcipher_request(struct device *dev, struct ablkcipher_request *req);

void unmap_ablkcipher_request(struct device *dev, struct ablkcipher_request *req);

int map_aead_request(struct device *dev, struct aead_request *req);

void unmap_aead_request(struct device *dev, struct aead_request *req);

int map_ahash_request_final(struct device *dev, struct ahash_request *req, bool do_update);

int map_ahash_request_update(struct device *dev, struct ahash_request *req);

void unmap_ahash_request(struct device *dev, struct ahash_request *req, bool do_revert);

void dx_sg_copy_part(u8 *dest, struct scatterlist *sg, int to_skip, unsigned int end, enum dx_sg_cpy_direct direct);

#endif /*__BUFFER_MGR_H__*/
