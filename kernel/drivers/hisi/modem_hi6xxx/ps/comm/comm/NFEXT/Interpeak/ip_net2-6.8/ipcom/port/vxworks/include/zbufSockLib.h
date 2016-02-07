/* zbufSockLib.h - zeroCopy buffer socket interface library header */

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
01d,20jun06,jpb  Removed I960 check
01c,04nov03,rlm  Ran batch header path update for header re-org.
01b,03nov03,rlm  Removed wrn/coreip/ prefix from #includes for header re-org.
01a,19nov02,nee  Ported to Accordion from ""zbufSockLib.h@@/main/1 from view tor3_x/tor3_x.synth".
*/

#ifndef __INCzbufSockLibh
#define __INCzbufSockLibh

#ifdef __cplusplus
extern "C" {
#endif

/* includes */
#include "zbufLib.h"

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern STATUS zbufSockLibInit (void);
extern int	zbufSockSend (int s, ZBUF_ID zbufId, int zbufLen, int flags);
extern int	zbufSockSendto (int s, ZBUF_ID zbufId, int zbufLen, int flags,
                    struct sockaddr *to, int tolen);
extern int	zbufSockBufSend (int s, char *buf, int bufLen,
                    VOIDFUNCPTR freeRtn, int freeArg, int flags);
extern int	zbufSockBufSendto (int s, char *buf, int bufLen,
                    VOIDFUNCPTR freeRtn, int freeArg, int flags,
                    struct sockaddr *to, int tolen);
extern ZBUF_ID	zbufSockRecv (int s, int flags, int *pLen);
extern ZBUF_ID	zbufSockRecvfrom (int s, int flags, int *pLen,
                    struct sockaddr *from, int *pFromLen);

#else	/* __STDC__ */

extern STATUS	zbufSockLibInit ();
extern int	zbufSockSend ();
extern int	zbufSockSendto ();
extern int	zbufSockBufSend ();
extern int	zbufSockBufSendto ();
extern ZBUF_ID	zbufSockRecv ();
extern ZBUF_ID	zbufSockRecvfrom ();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCzbufSockLibh */
