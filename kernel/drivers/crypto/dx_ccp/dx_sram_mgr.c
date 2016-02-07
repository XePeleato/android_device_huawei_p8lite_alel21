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

#include "dx_driver.h"
#include "dx_sram_mgr.h"

#if (DX_SEP_SRAM_SIZE == 0)
#error DX_SEP_SRAM_SIZE undefined.
#endif

/**
 * struct dx_sram_mgr_ctx -Internal RAM context manager
 * @ofs:   the offset to the non-allocated area
 */
struct dx_sram_mgr_ctx {
	uint32_t sram_base_addr;
};


/*!
 * Uninits SRAM pool.
 *
 * \param drvdata
 */
void dx_sram_mgr_fini(struct dx_drvdata *drvdata)
{
	struct dx_sram_mgr_ctx *smgr_ctx = drvdata->sram_mgr_handle;

	/* Free "this" context */
	if (smgr_ctx != NULL) {
		memset(smgr_ctx, 0, sizeof(struct dx_sram_mgr_ctx));
		kfree(smgr_ctx);
	}
}

/*!
 * Initializes SRAM pool.
 * The first X bytes of SRAM are reserved for ROM usage, hence, pool
 * starts right after X bytes.
 *
 * \param drvdata
 *
 * \return int Zero for success, negative value otherwise.
 */
int dx_sram_mgr_init(struct dx_drvdata *drvdata)
{
	struct dx_sram_mgr_ctx *smgr_ctx;
	int rc;

	/* Allocate "this" context */
	drvdata->sram_mgr_handle = kzalloc(
			sizeof(struct dx_sram_mgr_ctx), GFP_KERNEL);
	if (!drvdata->sram_mgr_handle) {
		DX_LOG_ERR("Not enough memory to allocate SRAM_MGR ctx(%d)\n",
			sizeof(struct dx_sram_mgr_ctx));
		rc = -ENOMEM;
		goto out;
	}
	smgr_ctx = drvdata->sram_mgr_handle;

	/* Pool starts after ROM bytes */
	smgr_ctx->sram_base_addr = READ_REGISTER(drvdata->cc_base +
			DX_CC_REG_OFFSET(HOST_RGF, HOST_SEP_SRAM_THRESHOLD));
	if (smgr_ctx->sram_base_addr & 0x3) {
		DX_LOG_ERR("Invalid SRAM threshold (addr=0x%08X)\n",
			(unsigned int)smgr_ctx->sram_base_addr);
		rc = -ENODEV;
		goto out;
	}

	return 0;

out:
	dx_sram_mgr_fini(drvdata);
	return rc;
}

/*!
 * Allocated buffer from SRAM pool.
 * Note: Caller is responsible to free the LAST allocated buffer.
 * This function does not taking care of any fragmentation may occur
 * by the order of calls to alloc/free.
 *
 * \param drvdata
 * \param size The requested bytes to allocate
 */
uint32_t *dx_sram_mgr_alloc(struct dx_drvdata *drvdata, uint32_t size)
{
	struct dx_sram_mgr_ctx *smgr_ctx = drvdata->sram_mgr_handle;
	uint32_t *p;

	if ((size > (DX_SEP_SRAM_SIZE - smgr_ctx->sram_base_addr)) ||
	    (size & 0x3))
		return NULL;

	p = (uint32_t *)smgr_ctx->sram_base_addr;
	smgr_ctx->sram_base_addr += size;
	DX_LOG_DEBUG("Allocated %dB @%p\n", size, p);
	return p;
}

/*!
 * Copy array of const values into a buffer resides in SRAM.
 * Note: each const value can't exceed word size.
 *
 * \param src Array of const values, each value must not exceed word size.
 * \param dst The target SRAM buffer to copy into
 * \param elements The number of const-values in "src" array
 * \param seq A pointer to the given IN/OUT descriptor sequence
 * \param seq_len A pointer to the given IN/OUT sequence length
 */
void copy_const_to_sram(
	const uint32_t *src, uint32_t *dst, int elements,
	HwDesc_s *seq, unsigned int *seq_len)
{
	uint32_t i;
	unsigned int idx = *seq_len;

	for (i = 0; i < elements; i++, idx++) {
		HW_DESC_INIT(&seq[idx]);
		HW_DESC_SET_DIN_CONST(&seq[idx], src[i], sizeof(uint32_t));
		HW_DESC_SET_DOUT_SRAM(&seq[idx], dst + i, sizeof(uint32_t));
		HW_DESC_SET_FLOW_MODE(&seq[idx], BYPASS);
	}

	*seq_len = idx;
}
