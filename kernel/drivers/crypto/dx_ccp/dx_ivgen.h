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

#include "hw_queue_defs.h"


#define DX_IVPOOL_SEQ_LEN 12

/*!
 * Allocates iv-pool and maps resources.
 * This function generates the first IV pool.
 *
 * \param drvdata Driver's private context
 *
 * \return int Zero for success, negative value otherwise.
 */
int dx_ivgen_init(struct dx_drvdata *drvdata);

/*!
 * Free iv-pool and ivgen context.
 *
 * \param drvdata
 */
void dx_ivgen_fini(struct dx_drvdata *drvdata);

/*!
 * Generates the initial pool in SRAM.
 * This function should be invoked when resuming DX driver.
 *
 * \param drvdata
 *
 * \return int Zero for success, negative value otherwise.
 */
int dx_ivgen_init_sram_pool(struct dx_drvdata *drvdata);

/*!
 * Acquires 16 Bytes IV from the iv-pool
 *
 * \param drvdata Driver private context
 * \param iv_out_dma The phys. IV out address
 * \param iv_out_size May be 8 or 16 bytes long
 * \param iv_seq IN/OUT array to the descriptors sequence
 * \param iv_seq_len IN/OUT pointer to the sequence length
 *
 * \return int Zero for success, negative value otherwise.
 */
int dx_ivgen_getiv(
	struct dx_drvdata *drvdata,
	dma_addr_t iv_out_dma,
	unsigned int iv_out_size,
	HwDesc_s iv_seq[],
	unsigned int *iv_seq_len);
