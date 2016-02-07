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

/* \file dx_cipher.h
   Discretix Cipher Crypto API
 */

#ifndef __DX_CIPHER_H__
#define __DX_CIPHER_H__

#include <linux/kernel.h>
#include <crypto/algapi.h>


struct ablkcipher_req_ctx {
	struct async_gen_req_ctx gen_ctx;
	enum dx_req_dma_buf_type dma_buf_type;
	unsigned int in_nents;
	unsigned int out_nents;
	struct mlli_params mlli_params;
	enum dx_secure_dir_type sec_dir;
};



int dx_ablkcipher_alloc(struct dx_drvdata *drvdata);

int dx_ablkcipher_free(struct dx_drvdata *drvdata);

#endif /*__DX_CIPHER_H__*/
