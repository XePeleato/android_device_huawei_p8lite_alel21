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

/* \file dx_config.h
   Definitions for Discretix Linux Crypto Driver
 */

#ifndef __DX_CONFIG_H__
#define __DX_CONFIG_H__

#include <linux/spinlock.h>
#include <asm/timex.h>
#include "dx_reg_common.h"

/* Supported CryptoCell product signature values */
/* Used to detect product ID at compile time */
#define DX_CC441P_SIG	0xDCC5E000
#define DX_CC442P_SIG	0xDCC44200

#define DRV_MODULE_VERSION "1.0"

/*HI3630: when sccc, should define DISABLE_COHERENT_DMA_OPS*/
//#define DISABLE_COHERENT_DMA_OPS
//#define FLUSH_CACHE_ALL
//#define COMPLETION_DELAY
//#define DX_DUMP_DESCS
//#define DX_DUMP_BYTES
//#define DX_DEBUG
//#define ENABLE_CYCLE_COUNT
//#define DX_IRQ_DELAY 100000

/* Define the CryptoCell DMA cache coherency signals configuration */
#ifdef DISABLE_COHERENT_DMA_OPS
/* Software Controlled Cache Coherency (SCCC) */
/*get back to 0x000 as Discretix's recommendation*/
#ifndef CONFIG_ARCH_HI3630
#define DX_CACHE_PARAMS (0x333)
#else
#define DX_CACHE_PARAMS (0x000)
#endif
#else
/* Hardware Controlled Cache Coherency (HCCC) */
#if (DX_DEV_SIGNATURE == DX_CC441P_SIG)
#define DX_CACHE_PARAMS (0x772)
#else /* DX_CC442P_SIG */
#define DX_CACHE_PARAMS (0x277)
#endif
#endif

#endif /*__DX_CONFIG_H__*/
