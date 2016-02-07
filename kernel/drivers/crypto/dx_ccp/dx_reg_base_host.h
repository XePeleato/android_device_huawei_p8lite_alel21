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
#ifndef __DX_REG_BASE_HOST_H__
#define __DX_REG_BASE_HOST_H__

/* Identify platform: Xilinx Zynq7000 ZC706 */
#ifndef CONFIG_ARCH_HI3630
#define DX_PLAT_ZYNQ7000 1
#define DX_PLAT_ZYNQ7000_ZC706 1
#endif

/* SEP core clock frequency in MHz */
#define DX_SEP_FREQ_MHZ 50

#ifndef CONFIG_ARCH_HI3630
#define DX_BASE_CC 0x80000000
#else
#define DX_BASE_CC 0xFF010000
#endif

#ifndef CONFIG_ARCH_HI3630
#define DX_BASE_ENV_REGS 0x40008000
#define DX_BASE_ENV_CC_MEMORIES 0x40008000
#define DX_BASE_ENV_PERF_RAM 0x40009000
#endif

#define DX_BASE_HOST_RGF 0x0UL
#define DX_BASE_CRY_KERNEL     0x0UL
#define DX_BASE_ROM     0x40000000

#endif /*__DX_REG_BASE_HOST_H__*/
