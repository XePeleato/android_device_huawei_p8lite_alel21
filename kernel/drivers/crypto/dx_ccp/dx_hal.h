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

/* pseudo dx_hal.h for cc44_perf_test_driver (to be able to include code from CC drivers) */

#ifndef __DX_HAL_H__
#define __DX_HAL_H__

#include <linux/io.h>

#ifdef CONFIG_ARM
#define READ_REGISTER(_addr) ioread32((const volatile void __iomem *)(_addr))
#define WRITE_REGISTER(_addr, _data)  iowrite32(_data, \
	(volatile void __iomem *)(_addr))
#else /* Default PPC */
#define READ_REGISTER(_addr) __raw_readl((const volatile void __iomem *)(_addr))
#define WRITE_REGISTER(_addr, _data)  __raw_writel(_data, \
	(volatile void __iomem *)(_addr))
#endif

#define DX_HAL_WriteCcRegister(offset, val) WRITE_REGISTER(cc_base + offset, val)
#define DX_HAL_ReadCcRegister(offset) READ_REGISTER(cc_base + offset)

#endif
