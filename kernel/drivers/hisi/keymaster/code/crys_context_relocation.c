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

#define DX_PAL_LOG_CUR_COMPONENT DX_LOG_MASK_CRYS_API

/*! \file CRYS_context_relocation.c
 * Handle relocation of crypto context in the context buffer given
 * by the user to assure it does not cross a page boundary
 */
/*#include <stdio.h>
#include <string.h>*/
#include "dx_pal_types.h"
#include "dx_pal_mem.h"
#include "crys_context_relocation.h"

/* Assume standard 4KB page size */
#define PAGE_SHIFT 12
#ifndef PAGE_SIZE
#define PAGE_SIZE (1<<PAGE_SHIFT)
#endif
#define PAGE_MASK (~(PAGE_SIZE-1))
/* "natural" 4B alignment */
#define CONTEXT_ALIGNMENT_SHIFT 2
#define CONTEXT_ALIGNMENT_SIZE (1<<CONTEXT_ALIGNMENT_SHIFT)
#define CONTEXT_ALIGNMENT_MASK (~((1<<CONTEXT_ALIGNMENT_SHIFT) - 1))
#define CONTEXT_ALIGN(addr) \
    (((unsigned long)(addr)+CONTEXT_ALIGNMENT_SIZE-1) & CONTEXT_ALIGNMENT_MASK)

#define IS_BUF_CROSS_PAGE(start, size) \
    (((unsigned long)(start) >> PAGE_SHIFT) < (((unsigned long)(start) + (size) - 1) >> PAGE_SHIFT))

/* Context buffer properties */
/* this data is always saved at the original start of user context buffer */
typedef struct {
	unsigned long bufSize;	/* Original user buffer size in bytes */
	unsigned long ctxSize;	/* Contained context actual size in bytes */
	unsigned long ctxOffset;/* Byte offset of the contained context */
} CRYS_CtxBufProps_t;

/*! 
 * Find a good offset in given buffer to accomodate given context size
 * without crossing a page boundary
 * Note: this function does not take into account the "bufProps" data
 *       that we locate in the buffer's start, so it should get 
 *       bufferStart at the location that follows that data.
 *
 * \param bufferStart The pointer to the context buffer given by the user
 *                     (offseted to accomodate the bufProps data)
 * \param bufferSize The total size of pointed buffer
 * \param contextSize The size of a context to place in the buffer
 *
 * \return Offset of the context in the given buffer
 */
static unsigned long GetNonCrossingOffset(unsigned long bufferStart,
					  unsigned long bufferSize, 
					  unsigned long contextSize)
{
	const unsigned long bufStartNextPage = 
	(bufferStart + PAGE_SIZE) & PAGE_MASK;
	const unsigned long bufEndPage =
	(bufferStart + bufferSize - 1) & PAGE_MASK;
	unsigned long goodLocation;

	if (bufStartNextPage > bufEndPage) {
		/* Buffer does not cross a page */
		/* Just assure alignment of buffer start */
		goodLocation = CONTEXT_ALIGN(bufferStart);
	} else if (bufStartNextPage == bufEndPage) {
		/* Buffer crosses one page boundary */
		/* Return part that can accomodate context */
		goodLocation = CONTEXT_ALIGN(bufferStart);
		if ((bufStartNextPage - goodLocation) < contextSize) {
			/* First part is too small, pick the start of the second page */
			goodLocation = bufEndPage; /* Page is always aligned... */
		}
	} else {
		/* Buffer crosses two page boundaries */
		/* Pick the start of the full page in the middle */
		goodLocation = bufStartNextPage;
	}

#ifdef DEBUG_CONTEXT
	/*printf("GetNonCrossingOffset: start=0x%08lX buf_size=%lu ctx_size=%lu ==> offset=0x%lX\n",
	       bufferStart, bufferSize, contextSize, 
	       goodLocation - bufferStart);*/
#endif

	return goodLocation - bufferStart;
}

/*!
 * Initialize the context offset for a new buffer given to INIT phase
 * 
 * \param bufferStart The address of the context buffer given by the user
 * \param bufferSize The size of the user buffer in bytes
 * \param contextSize The required size (in bytes) of the context
 *
 * \return The address of the context within the buffer
 */
void *DX_InitUserCtxLocation(void *bufferStart,
			     unsigned long bufferSize, 
			     unsigned long contextSize)
{
	/* Buffer must accomodate the BufProps and 2*contextSize to 
	   assure at least contextSize bytes are not crossing page boundary */
	const unsigned long requested_buf_size = 
		sizeof(CRYS_CtxBufProps_t) + 2*contextSize;
	unsigned long contextOffset;
	void *contextStart;
	CRYS_CtxBufProps_t *bufProps = (CRYS_CtxBufProps_t *)bufferStart;
	/* Buffer properties are save at reserved space at buffer's start */

	/* Verify given sizes validity*/
	if ((contextSize > PAGE_SIZE) || (bufferSize < requested_buf_size)) {
#ifdef DEBUG_CONTEXT
		fprintf(stderr, "DX_InitUserCtxLocation: Given buffer size (%lu B) "
			"is too small for required context size (%lu B) - "
			"must be at least %lu B\n",
			bufferSize, contextSize, requested_buf_size);
#endif
		return NULL;
	}

	/* Get good location (starting from buffer_ptr + sizeof(void*))*/
	contextOffset = GetNonCrossingOffset((unsigned long)bufferStart +
					     sizeof(CRYS_CtxBufProps_t),
					     bufferSize, contextSize);
	/* The actual offset is after the CRYS_CtxBufProps_t structure */
	contextOffset += sizeof(CRYS_CtxBufProps_t);
	/* Save buffer properties */
	bufProps->bufSize = bufferSize;
	bufProps->ctxSize = contextSize;
	bufProps->ctxOffset = contextOffset;

	contextStart = (void*)((unsigned long)bufferStart + contextOffset);
	return contextStart;
}

/*!
 * Return the context address in the given buffer
 * If previous context offset is now crossing a page the context data
 * would be moved to a good location.
 *
 * \param bufferStart The address of the context buffer given by the user
 *
 * \return The address of the context within the buffer
 */
void *DX_GetUserCtxLocation(void *bufferStart)
{
	/* Calculate current context location based on offset in buffer props */
	CRYS_CtxBufProps_t *bufProps = (CRYS_CtxBufProps_t *)bufferStart;
	void *curContextLocation = (void *)
				   ((unsigned long)bufferStart + bufProps->ctxOffset);
	unsigned long newContextOffset;
	void *newContextLocation;

	/* Verify current location */
	if (!IS_BUF_CROSS_PAGE(curContextLocation, bufProps->ctxSize)) {
		/* If context does not cross page boundary - keep it where it is */
		return curContextLocation;
	}

	/* If current location crosses a page boundary, find a new location */
	newContextOffset = GetNonCrossingOffset(
					       (unsigned long)bufferStart + sizeof(CRYS_CtxBufProps_t),
					       bufProps->bufSize, bufProps->ctxSize);   
	/* The actual offset is after the bufProps structure */
	newContextOffset += sizeof(CRYS_CtxBufProps_t);
	newContextLocation = (void*)((unsigned long)bufferStart + newContextOffset);

	/* memmove context from original location to new location */
	DX_PAL_MemMove(newContextLocation, curContextLocation, bufProps->ctxSize);
	/* update new location in start of buffer */
	bufProps->ctxOffset = newContextOffset;

	return newContextLocation;
}

