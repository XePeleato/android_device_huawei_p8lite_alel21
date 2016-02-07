/* zbufLib.h - zeroCopy buffer interface library header */

/* 
 * Copyright 1984-2006,2008 Wind River Systems, Inc.
 *
 * The right to copy, distribute or otherwise make use of this software
 * may be licensed only pursuant to the terms of an applicable Wind River
 * license agreement.
 */

/*
modification history
--------------------
01g,28jan08,aes  complete revamp. Using Ipcom_pkt instead of mbuf.
01c,20jun06,jpb  Removed I960 check
01b,17mar04,rp   updated to build for 5.5
01a,19nov02,nee  Ported to Accordion from ""zbufLib.h@@/main/1 from view tor3_x/tor3_x.synth".
*/

#ifndef __INCzbufLibh
#define __INCzbufLibh

#ifdef __cplusplus
extern "C" {
#endif

#include <limits.h>

#include <semLib.h>
#include <intLib.h>

#include <ipcom_pkt.h>
#include <ipcom_netif.h>

/* typedefs */

typedef int			    ZBUF_ID;
typedef Ipcom_pkt*		ZBUF_SEG;

/* status codes */
#define S_zbufLib_ID_INVALID            (M_mbufLib | 1)
#define S_zbufLib_ID_EMPTY              (M_mbufLib | 2)
#define S_zbufLib_SEGMENT_NOT_FOUND     (M_mbufLib | 3)
#define S_zbufLib_LENGTH_INVALID        (M_mbufLib | 4)
#define S_zbufLib_OFFSET_INVALID        (M_mbufLib | 5)


/* defines */
#define	ZBUF_BEGIN		-INT_MAX	/* shortcut for prepending */
#define	ZBUF_END		INT_MAX		/* shortcut for appending */
#define	ZBUF_NONE		((ZBUF_SEG) NONE) /* cut past zbuf */

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)
extern STATUS	zbufLibInit (FUNCPTR libInitRtn);
extern ZBUF_ID	zbufCreate (void);
extern STATUS	zbufDelete (ZBUF_ID zbufId);
extern ZBUF_SEG	zbufInsert (ZBUF_ID zbufId1, ZBUF_SEG zbufSeg, int offset,
                    ZBUF_ID zbufId2);
extern ZBUF_SEG	zbufInsertBuf (ZBUF_ID zbufId, ZBUF_SEG zbufSeg, int offset,
		    caddr_t buf, int len, VOIDFUNCPTR freeRtn, int freeArg);
extern ZBUF_SEG	zbufInsertCopy (ZBUF_ID zbufId, ZBUF_SEG zbufSeg,
		    int offset, caddr_t buf, int len);
extern int	zbufExtractCopy (ZBUF_ID zbufId, ZBUF_SEG zbufSeg,
		    int offset, caddr_t buf, int len);
extern ZBUF_SEG	zbufCut (ZBUF_ID zbufId, ZBUF_SEG zbufSeg, int offset, int len);
extern ZBUF_ID	zbufSplit (ZBUF_ID zbufId, ZBUF_SEG zbufSeg, int offset);
extern ZBUF_ID	zbufDup (ZBUF_ID zbufId, ZBUF_SEG zbufSeg, int offset, int len);
extern int	zbufLength (ZBUF_ID zbufId);
extern ZBUF_SEG	zbufSegFind (ZBUF_ID zbufId, ZBUF_SEG zbufSeg, int *pOffset);
extern ZBUF_SEG	zbufSegNext (ZBUF_ID zbufId, ZBUF_SEG zbufSeg);
extern ZBUF_SEG	zbufSegPrev (ZBUF_ID zbufId, ZBUF_SEG zbufSeg);
extern caddr_t	zbufSegData (ZBUF_ID zbufId, ZBUF_SEG zbufSeg);
extern int	zbufSegLength (ZBUF_ID zbufId, ZBUF_SEG zbufSeg);

#else	/* __STDC__ */

extern STATUS	zbufLibInit ();
extern ZBUF_ID	zbufCreate ();
extern STATUS	zbufDelete ();
extern ZBUF_SEG	zbufInsert ();
extern ZBUF_SEG	zbufInsertBuf ();
extern ZBUF_SEG	zbufInsertCopy ();
extern int	zbufExtractCopy ();
extern ZBUF_SEG	zbufCut ();
extern ZBUF_ID	zbufSplit ();
extern ZBUF_ID	zbufDup ();
extern int	zbufLength ();
extern ZBUF_SEG	zbufSegFind ();
extern ZBUF_SEG	zbufSegNext ();
extern ZBUF_SEG	zbufSegPrev ();
extern caddr_t	zbufSegData ();
extern int	zbufSegLength ();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCzbufLibh */
