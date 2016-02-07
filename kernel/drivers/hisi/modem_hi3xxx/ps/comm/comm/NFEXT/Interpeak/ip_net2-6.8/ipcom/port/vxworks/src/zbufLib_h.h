/* zbufLib_h.h - zeroCopy buffer "private" header */

/* 
 * Copyright 2008 Wind River Systems, Inc.
 *
 * The right to copy, distribute or otherwise make use of this software
 * may be licensed only pursuant to the terms of an applicable Wind River
 * license agreement.
 */

/*
modification history
--------------------
01g,29jan08,aes  Created to make the cut between zbufLib and zbufSockLib nicer.
*/

#ifndef _zbufLib_h__h_
#define _zbufLib_h__h_

#ifdef __cplusplus
extern "C" {
#endif


#include <ipcom_pkt.h>
#include <ipcom_netif.h>

/* prototypes for zbufSockLib */
ZBUF_SEG* _zbufGetSeg(int);
void _zbufSetSeg(int, Ipcom_pkt* pkt);
void _zbufDeleteEmpty(int);

#define ZBUF_GETSEG(x)               *_zbufGetSeg(x)
#define ZBUF_SETSEG(x, seg)          _zbufSetSeg(x, seg)
#define ZSEG_NEXT(pkt)               ((Ipcom_pkt *)pkt)->next_part


#define ZBUF_ID_DELETE_EMPTY(x)      _zbufDeleteEmpty(x)
#define ZBUF_ID_DELETE(x)            zbufDelete(x)
#define ZBUF_ID_CREATE(x)            {  x = zbufCreate(); }




#ifdef __cplusplus
}
#endif

#endif /* _zbufLib_h__h_ */

