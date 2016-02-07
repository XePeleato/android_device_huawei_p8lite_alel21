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
 * \file dx_cc_regs.h
 * \brief Macro definitions for accessing Dx CryptoCell register space
 *
 * For SeP code define DX_CC_SEP
 * For Host REE (Rich Execution Environment, a.k.a., HLOS) environment (e.g., Linux) define DX_CC_REE
 * For Host TEE environment (secure OS/TZ) define DX_CC_TEE
 */

#ifndef _DX_CC_REGS_H_
#define _DX_CC_REGS_H_

#include "dx_bitops.h"

/* Register Offset macro */
#define DX_CC_REG_OFFSET(unit_name, reg_name)               \
	(DX_BASE_ ## unit_name + DX_ ## reg_name ## _REG_OFFSET)

/* Register Offset macros (from registers base address in host) */
#if defined(DX_CC_REE) || defined(DX_CC_TEE)

#include "dx_reg_base_host.h"

/* Indexed GPR offset macros - note the (not original) preprocessor tricks...*/
/* (Using the macro without the "_" prefix is allowed with another macro      *
 *  as the gpr_idx) */
#define _SEP_HOST_GPR_REG_OFFSET(gpr_idx) \
	DX_CC_REG_OFFSET(HOST_RGF, HOST_SEP_HOST_GPR ## gpr_idx)
#define SEP_HOST_GPR_REG_OFFSET(gpr_idx) _SEP_HOST_GPR_REG_OFFSET(gpr_idx)
#define _HOST_SEP_GPR_REG_OFFSET(gpr_idx) \
	DX_CC_REG_OFFSET(HOST_RGF, HOST_HOST_SEP_GPR ## gpr_idx)
#define HOST_SEP_GPR_REG_OFFSET(gpr_idx) _HOST_SEP_GPR_REG_OFFSET(gpr_idx)

/* GPR IRQ bit shift/mask by GPR index */
#define _SEP_HOST_GPR_IRQ_SHIFT(gpr_idx) \
	DX_HOST_IRR_SEP_HOST_GPR ## gpr_idx ## _INT_BIT_SHIFT
#define SEP_HOST_GPR_IRQ_SHIFT(gpr_idx) \
	_SEP_HOST_GPR_IRQ_SHIFT(gpr_idx)
#define SEP_HOST_GPR_IRQ_MASK(gpr_idx) \
	(1 << SEP_HOST_GPR_IRQ_CAUSE_SHIFT(gpr_idx))

/* Read-Modify-Write a field of a register */
#define MODIFY_REGISTER_FLD(unitName, regName, fldName, fldVal)         \
do {								            \
	uint32_t regVal;						    \
	regVal = READ_REGISTER(DX_CC_REG_ADDR(unitName, regName));       \
	DX_CC_REG_FLD_SET(unitName, regName, fldName, regVal, fldVal); \
	WRITE_REGISTER(DX_CC_REG_ADDR(unitName, regName), regVal);       \
} while (0)

#elif defined(DX_CC_SEP)

#include "dx_reg_base_sep.h"

/* We only provide this macro for SEP code because other platforms require usage
   of (mapping base + offset), i.e., use of DX_CC_REG_OFFSET */
/* PLEASE AVOID USING THIS MACRO FOR NEW SEP CODE. USE THE OFFSET MACROS. */
#define DX_CC_REG_ADDR(unit_name, reg_name)            \
	 (DX_BASE_CC_PERIF + DX_BASE_ ## unit_name + \
	  DX_ ## reg_name ## _REG_OFFSET)

/* Indexed GPR address macros - note the (not original) preprocessor tricks...*/
/* (Using the macro without the "_" prefix is allowed with another macro      *
 *  as the gpr_idx) */
#define _SEP_HOST_GPR_REG_ADDR(gpr_idx) \
	DX_CC_REG_ADDR(SEP_RGF, SEP_SEP_HOST_GPR ## gpr_idx)
#define SEP_HOST_GPR_REG_ADDR(gpr_idx) _SEP_HOST_GPR_REG_ADDR(gpr_idx)
#define _HOST_SEP_GPR_REG_ADDR(gpr_idx) \
	DX_CC_REG_ADDR(SEP_RGF, SEP_HOST_SEP_GPR ## gpr_idx)
#define HOST_SEP_GPR_REG_ADDR(gpr_idx) _HOST_SEP_GPR_REG_ADDR(gpr_idx)

#else
#error Execution domain is not DX_CC_SEP/DX_CC_REE/DX_CC_TEE
#endif

/* Registers address macros for ENV registers (development FPGA only) */
#ifdef DX_BASE_ENV_REGS

#ifndef DX_CC_SEP /* Irrelevant for SeP code */
/* This offset should be added to mapping address of DX_BASE_ENV_REGS */
#define DX_ENV_REG_OFFSET(reg_name) (DX_ENV_ ## reg_name ## _REG_OFFSET)
#endif

#endif /*DX_BASE_ENV_REGS*/

/* Bit fields access */
#define DX_CC_REG_FLD_GET(unit_name, reg_name, fld_name, reg_val)	      \
	(DX_ ## reg_name ## _ ## fld_name ## _BIT_SIZE == 0x20 ?	      \
	reg_val /* Optimization for 32b fields */ :			      \
	BITFIELD_GET(reg_val, DX_ ## reg_name ## _ ## fld_name ## _BIT_SHIFT, \
		     DX_ ## reg_name ## _ ## fld_name ## _BIT_SIZE))

#define DX_CC_REG_FLD_SET(                                               \
	unit_name, reg_name, fld_name, reg_shadow_var, new_fld_val)      \
do {                                                                     \
	if (DX_ ## reg_name ## _ ## fld_name ## _BIT_SIZE == 0x20)       \
		reg_shadow_var = new_fld_val; /* Optimization for 32b fields */\
	else                                                             \
		BITFIELD_SET(reg_shadow_var,                             \
			DX_ ## reg_name ## _ ## fld_name ## _BIT_SHIFT,  \
			DX_ ## reg_name ## _ ## fld_name ## _BIT_SIZE,   \
			new_fld_val);                                    \
} while (0)

/* Usage example:
   uint32_t reg_shadow = READ_REGISTER(DX_CC_REG_ADDR(CRY_KERNEL,AES_CONTROL));
   DX_CC_REG_FLD_SET(CRY_KERNEL,AES_CONTROL,NK_KEY0,reg_shadow, 3);
   DX_CC_REG_FLD_SET(CRY_KERNEL,AES_CONTROL,NK_KEY1,reg_shadow, 1);
   WRITE_REGISTER(DX_CC_REG_ADDR(CRY_KERNEL,AES_CONTROL), reg_shadow);
 */

#endif /*_DX_CC_REGS_H_*/
