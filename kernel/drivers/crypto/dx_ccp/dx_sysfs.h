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

/* \file dx_sysfs.h
   Discretix sysfs APIs
 */

#ifndef __DX_SYSFS_H__
#define __DX_SYSFS_H__

#include <asm/timex.h>
#define SEP_SUPPORT_SHA 256
#include "sep_ctx.h"

enum stat_phase {
	STAT_PHASE_0 = 0,
	STAT_PHASE_1,
	STAT_PHASE_2,
	STAT_PHASE_3,
	STAT_PHASE_4,
	STAT_PHASE_5,
	MAX_STAT_PHASES,
};
enum stat_op {
	STAT_OP_TYPE_ENCODE = SEP_CRYPTO_DIRECTION_ENCRYPT,
	STAT_OP_TYPE_DECODE = SEP_CRYPTO_DIRECTION_DECRYPT,
	STAT_OP_TYPE_SETKEY,
	STAT_OP_TYPE_GENERIC,
	MAX_STAT_OP_TYPES,
};

void init_stat_db(void);

void update_stat(unsigned int op_type, unsigned int phase, cycles_t result);

void display_stat_db(void);

#endif /*__DX_SYSFS_H__*/
