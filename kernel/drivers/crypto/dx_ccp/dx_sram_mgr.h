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


/*!
 * Initializes SRAM pool.
 * The first X bytes of SRAM are reserved for ROM usage, hence, pool
 * starts right after X bytes.
 *
 * \param drvdata
 *
 * \return int Zero for success, negative value otherwise.
 */
int dx_sram_mgr_init(struct dx_drvdata *drvdata);

/*!
 * Uninits SRAM pool.
 *
 * \param drvdata
 */
void dx_sram_mgr_fini(struct dx_drvdata *drvdata);

/*!
 * Allocated buffer from SRAM pool.
 * Note: Caller is responsible to free the LAST allocated buffer.
 * This function does not taking care of any fragmentation may occur
 * by the order of calls to alloc/free.
 *
 * \param drvdata
 * \param size The requested bytes to allocate
 */
uint32_t *dx_sram_mgr_alloc(struct dx_drvdata *drvdata, uint32_t size);

/*!
 * Copy array of const values into a buffer resides in SRAM.
 * Note: each const value can't exceed word size.
 *
 * \param src A pointer to array of const values, each value must not exceed word size.
 * \param dst The target SRAM buffer to copy into
 * \param elements The number of const-values in "src" array
 * \param seq A pointer to the given IN/OUT descriptor sequence
 * \param seq_len A pointer to the given IN/OUT sequence length
 */
void copy_const_to_sram(
	const uint32_t *src, uint32_t *dst, int elements,
	HwDesc_s *seq, unsigned int *seq_len);
