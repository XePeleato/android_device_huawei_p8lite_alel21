/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This software is protected by copyright, international           *
* treaties and patents.                                            *
* Use of this Software as part of or with the Discretix CryptoCell *
* or Packet Engine products is governed by the products'           *
* commercial end user license agreement ("EULA").                  *
* It is possible that copies of this Software might be distributed *
* under some type of GNU General Public License ("GPL").           *
* Notwithstanding any such distribution under the terms of GPL,    *
* GPL does not govern the use of this Software as part of or with  *
* the Discretix CryptoCell or Packet Engine products, for which a  *
* EULA is required.                                                *
* If the product's EULA allows any copy or reproduction of this    *
* Software, then such copy or reproduction must include this       *
* Copyright Notice as well as any other notices provided           *
* thereunder.                                                      *
********************************************************************/


#ifndef _SEP_LLI_H_
#define _SEP_LLI_H_
#ifdef __KERNEL__
#include <linux/types.h>
#else
//#include <stdint.h>
#endif
#include "dx_bitops.h"

#define SEP_LLI_GET(lli_p, lli_field) BITFIELD_GET(                            \
		((uint32_t *)(lli_p))[SEP_LLI_ ## lli_field ## _WORD_OFFSET],  \
		SEP_LLI_ ## lli_field ## _BIT_OFFSET,			       \
		SEP_LLI_ ## lli_field ## _BIT_SIZE)
#define SEP_LLI_SET(lli_p, lli_field, new_val) BITFIELD_SET(                   \
		((uint32_t *)(lli_p))[SEP_LLI_ ## lli_field ## _WORD_OFFSET],  \
		SEP_LLI_ ## lli_field ## _BIT_OFFSET,			       \
		SEP_LLI_ ## lli_field ## _BIT_SIZE,			       \
		new_val)

#define SEP_LLI_INIT(lli_p)  do { \
	((uint32_t *)(lli_p))[0] = 0; \
	((uint32_t *)(lli_p))[1] = 0; \
	} while (0)

/* Copy local LLI scratchpad to SeP LLI buffer */
#define SEP_LLI_COPY_TO_SEP(sep_lli_p, host_lli_p) do {             \
	int i;                                                      \
	for (i = 0; i < SEP_LLI_ENTRY_WORD_SIZE; i++)               \
		((uint32_t *)(sep_lli_p))[i] =                      \
			cpu_to_le32(((uint32_t *)(host_lli_p))[i]); \
} while (0)
/* and vice-versa */
#define SEP_LLI_COPY_FROM_SEP(host_lli_p, sep_lli_p) do {                 \
	int i;                                                            \
		for (i = 0; i < SEP_LLI_ENTRY_WORD_SIZE; i++)             \
			((uint32_t *)(host_lli_p))[i] =                   \
				le32_to_cpu(((uint32_t *)(sep_lli_p))[i]);\
} while (0)

/* Size of entry */
#define SEP_LLI_ENTRY_WORD_SIZE 2
#define SEP_LLI_ENTRY_BYTE_SIZE (SEP_LLI_ENTRY_WORD_SIZE * sizeof(uint32_t))

/* (DMA) Address: ADDR */
#define SEP_LLI_ADDR_WORD_OFFSET 0
#define SEP_LLI_ADDR_BIT_OFFSET 0
#define SEP_LLI_ADDR_BIT_SIZE 32
/* Size: SIZE */
#define SEP_LLI_SIZE_WORD_OFFSET 1
#define SEP_LLI_SIZE_BIT_OFFSET 0
#define SEP_LLI_SIZE_BIT_SIZE 30
/* First/Last LLI entries bit marks: FIRST, LAST */
#define SEP_LLI_FIRST_WORD_OFFSET 1
#define SEP_LLI_FIRST_BIT_OFFSET 30
#define SEP_LLI_FIRST_BIT_SIZE 1
#define SEP_LLI_LAST_WORD_OFFSET 1
#define SEP_LLI_LAST_BIT_OFFSET 31
#define SEP_LLI_LAST_BIT_SIZE 1

#endif /*_SEP_LLI_H_*/
