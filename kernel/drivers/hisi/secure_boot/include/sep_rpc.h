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

#ifndef __SEP_RPC_H__
#define __SEP_RPC_H__

/* SeP RPC infrastructure API */
#ifdef __KERNEL__
#include <linux/types.h>

#else
#include "dx_pal_types.h"

#endif /*__KERNEL__*/

/* Maximum size of SeP RPC message in bytes */
#define SEP_RPC_MAX_MSG_SIZE 8191

/* The maximum allowed user memory references per function
   (CRYS requires only 2, but GPAPI/TEE needs up to 4) */
#define SEP_RPC_MAX_MEMREF_PER_FUNC 4

/* If this macro is not provided by the includer of this file,
   the log message would be dropped */
#ifndef SEP_RPC_LOG
#define SEP_RPC_LOG(format, ...) do {} while (0)
#endif

#define SEP_RPC_ASSERT(cond, inval_param_retcode) if (!(cond)) { \
	SEP_RPC_LOG("SEP_RPC_ASSERT: %s\n", #cond);              \
	return inval_param_retcode;                              \
}

/* NOTE:
   All data must be in little (SeP) endian */

typedef enum seprpc_retcode {
	SEPRPC_RET_OK =           0,
	SEPRPC_RET_ERROR,         /*Generic error code (not one of the others)*/
	SEPRPC_RET_EINVAL_AGENT,  /* Unknown agent ID */
	SEPRPC_RET_EINVAL_FUNC,   /* Unknown function ID for given agent */
	SEPRPC_RET_EINVAL,        /* Invalid parameter */
	SEPRPC_RET_ENORSC,        /* Not enough resources to complete request */
	SEPRPC_RET_RESERVE32 = 0x7FFFFFFF /* assure this enum is 32b */
} seprpc_retcode_t;

/* The ID of a SeP RPC Agent (API name/index space) */
typedef uint16_t seprpc_agent_id_t;

/* A function ID (index?) in the scope of a given agent */
typedef uint16_t seprpc_func_id_t;

typedef enum seprpc_memref_type {
	SEPRPC_MEMREF_NULL  = 0, /* Invalid memory reference */
	SEPRPC_MEMREF_EMBED = 1, /* Data embedded in parameters message */
	SEPRPC_MEMREF_DLLI  = 2,
	SEPRPC_MEMREF_MLLI  = 3,
	SEPRPC_MEMREF_MAX = SEPRPC_MEMREF_MLLI,
	SEPRPC_MEMREF_RESERVE32 = 0x7FFFFFFF /* assure this enum is 32b */
} seprpc_memref_type_t;

/* A strcuture to pass host memory reference */
struct seprpc_memref {
	seprpc_memref_type_t ref_type;
	uint32_t location;
	uint32_t size;
	uint32_t count;
	/* SEPRPC_MEMREF_EMBED: location= offset in SepRpc_Params .
				size= data size in bytes. count= N/A */
	/* SEPRPC_MEMREF_DLLI: location= DMA address of data in host memory.
				size= data size in bytes. count= N/A. */
	/* SEPRPC_MEMREF_MLLI: location= DMA address of first MLLI table.
				size= size in bytes of first table.
				count= Num. of MLLI tables.*/
};

struct seprpc_params {
	uint32_t num_of_memrefs; /* Number of elements in the memRef array */
	struct seprpc_memref memref[1];
	/* This array is actually in the size of numOfMemRefs
	   (i.e., it is just a placeholder that may be void) */
	/*   Following this array comes the function-specific parameters */
} __attribute__((__may_alias__));



#endif /*__SEP_RPC_H__*/
