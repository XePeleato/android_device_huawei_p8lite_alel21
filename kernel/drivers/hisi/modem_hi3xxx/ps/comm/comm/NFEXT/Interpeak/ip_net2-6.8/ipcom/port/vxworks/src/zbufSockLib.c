/* zbufSockLib.c - zbuf socket interface library */

/* Copyright 1984 - 2005, 2008 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01g,28jan08,aes  complete revamp. Using Ipcom_pkt instead of mbuf.
01c,12jan05,vvv  osdep.h cleanup
01b,05nov03,rlm  Ran batch header update for header re-org.
01a,19nov02,nee  Ported to Accordion from "zbufSockLib.c@@/main/tor3_x/tor3_x.synth/1".
                 Eliminated a level of indirection to zbufSock* calls and removed redundant
                 file descriptor validation.
*/

/*
DESCRIPTION
This library contains routines that communicate over BSD sockets using
the
.I "zbuf interface"
described in the zbufLib manual page.  These zbuf
socket calls communicate over BSD sockets in a similar manner to the
socket routines in sockLib, but they avoid copying data unnecessarily
between application buffers and network buffers.

PROTECTION DOMAINS
This feature can only be used from within the kernel protection domain.

To use this feature, include the following component:
INCLUDE_ZBUF_SOCK

SEE ALSO
zbufLib, sockLib,
.I "VxWorks AE Network Programmer's Guide: Networking APIs"
*/

/* includes */

#include "ipcom_config.h"

#ifdef IPCOM_ZEROCOPY

#include <vxWorks.h>
#include <zbufLib.h>
#include "zbufLib_h.h"
#include <sys/socket.h>
#include <wrn/coreip/sockLib.h>
#include <zbufSockLib.h>
#include <errnoLib.h>
#include <sockFunc.h>
#include <netLib.h>

#include <ipcom_clib.h>
#include <ipcom_pkt.h>
#include <ipcom_netif.h>
#include <ipcom_os.h>
#include <ipnet_h.h>

#include <stdlib.h>


/*******************************************************************************
*
* zbufSockLibInit - initialize the zbuf socket interface library
*
* This routine initializes the zbuf socket interface
* library.  It must be called before any zbuf socket routines are used.
* It is called automatically when INCLUDE_ZBUF_SOCK is defined.
*
* PROTECTION DOMAINS
* This function can only be called from within the kernel protection domain.
*
* RETURNS:
* OK, or ERROR if the zbuf socket interface could not be initialized.
*/


STATUS zbufSockLibInit (void)
{
    return OK;
}

/*******************************************************************************
*
* zbufSockSend - send zbuf data
*
* This routine transmits all of the data in <zbufId> to a previously
* connected socket.
*
* The <zbufLen> parameter is used only for determining the amount of space
* needed from the socket write buffer.  <zbufLen> has no effect on how many
* bytes are sent; the entire zbuf is always transmitted.  If the length of
* <zbufId> is not known, the caller must first determine it by calling
* zbufLength().
*
* This routine transfers ownership of the zbuf from the user application
* to the VxWorks network stack.  The zbuf ID <zbufId> is deleted by this
* routine, and should not be used after the routine is called, even if
* an ERROR status is returned.  (Exceptions:  when the routine
* fails because the zbuf socket interface library was not initialized or an
* invalid zbuf ID was passed in, in which case there is no zbuf to delete.
* Moreover, if the call fails during a non-blocking I/O socket write
* with an `errno' of EWOULDBLOCK, then <zbufId> is not deleted; thus the
* caller may send it again at a later time.)
*
* You may OR the following values into the <flags> parameter with this
* operation:
*
* .iP "MSG_OOB (0x1)" 26
* Out-of-band data.
*
* .iP "MSG_DONTROUTE (0x4)"
* Send without using routing tables.
* .LP
*
* PROTECTION DOMAINS
* This function can only be called from within the kernel protection domain.
*
* RETURNS
* The number of bytes sent, or ERROR if the call fails.
*
* SEE ALSO: zbufLength(), zbufSockBufSend(), send()
*/

int zbufSockSend
    (
    int         s,          /* socket to send to */
    ZBUF_ID     zbufId,     /* zbuf to transmit */
    int         zbufLen,    /* length of entire zbuf */
    int         flags       /* flags to underlying protocols */
    )
{
    return zbufSockSendto(s, zbufId, zbufLen, flags, NULL, 0);
}

/*******************************************************************************
*
* zbufSockSendto - send a zbuf message
*
* This routine sends the entire message in <zbufId> to the socket
* named by <to>.  The socket <s> is the sending socket.
*
* The <zbufLen> parameter is used only for determining the amount of space
* needed from the socket write buffer.  <zbufLen> has no effect on how many
* bytes are sent; the entire zbuf is always transmitted.  If the length of
* <zbufId> is not known, the caller must first determine it by calling
* zbufLength().
*
* This routine transfers ownership of the zbuf from the user application
* to the VxWorks network stack.  The zbuf ID <zbufId> is deleted by this
* routine, and should not be used after the routine is called, even if
* an ERROR status is returned.  (Exceptions:  when the routine
* fails because the zbuf socket interface library was not initialized or an
* invalid zbuf ID was passed in, in which case there is no zbuf to delete.
* Moreover, if the call fails during a non-blocking I/O socket write
* with an `errno' of EWOULDBLOCK, then <zbufId> is not deleted; thus the
* caller may send it again at a later time.)
*
* You may OR the following values into the <flags> parameter with this
* operation:
*
* .iP "MSG_OOB (0x1)" 26
* Out-of-band data.
*
* .iP "MSG_DONTROUTE (0x4)"
* Send without using routing tables.
* .LP
*
* PROTECTION DOMAINS
* This function can only be called from within the kernel protection domain.
*
* RETURNS
* The number of bytes sent, or ERROR if the call fails.
*
* SEE ALSO: zbufLength(), zbufSockBufSendto(), sendto()
*/

int zbufSockSendto
    (
    int         s,          /* socket to send to */
    ZBUF_ID     zbufId,     /* zbuf to transmit */
    int         zbufLen,    /* length of entire zbuf */
    int         flags,      /* flags to underlying protocols */
    struct sockaddr *to,    /* recipient's address */
    int         tolen       /* length of <to> socket addr */
    )
{
    ZBUF_SEG    zbufSeg;        /* start of chain */
    int         retVal = 0;     /* sendto() return status */

    zbufSeg = ZBUF_GETSEG(zbufId);
    if (zbufSeg != NULL)    /* send zbuf chain */
        retVal = sendto (s, (char *) zbufSeg, zbufLen, flags | IP_MSG_ZBUF, to, tolen);

    /* do not delete zbuf ID on EWOULDBLOCK error so user can resend ID */
    if ((retVal != ERROR) || (errnoGet() != EWOULDBLOCK))
        ZBUF_ID_DELETE_EMPTY(zbufId);

    return retVal;
}

/*******************************************************************************
*
* zbufSockBufSend - create a zbuf from user data and send it to a socket
*
* This routine creates a zbuf from the user buffer <buf>, and transmits
* it to a previously connected socket.
*
* The user-provided free routine callback at <freeRtn> is called when <buf>
* is no longer in use by the network stack.  Applications can
* exploit this callback to receive notification that <buf> is free.
* If <freeRtn> is NULL, the routine functions normally, except that the
* application has no way of being notified when <buf> is released by the
* network stack.  The free routine runs in the context of the task that last
* references the buffer.  This is typically either the context of tNetTask,
* or the context of the caller's task.  Declare <freeRtn> as follows
* (using whatever name is convenient):
* .CS
*       void freeCallback
*           (
*           caddr_t     buf,    /@ pointer to user buffer @/
*           int         freeArg /@ user-provided argument to free routine @/
*           )
* .CE
*
* You may OR the following values into the <flags> parameter with this
* operation:
*
* .iP "MSG_OOB (0x1)" 26
* Out-of-band data.
*
* .iP "MSG_DONTROUTE (0x4)"
* Send without using routing tables.
* .LP
*
* PROTECTION DOMAINS
* This function can only be called from within the kernel protection domain.
*
* RETURNS:
* The number of bytes sent, or ERROR if the call fails.
*
* SEE ALSO
* zbufSockSend(), send()
*
*/

int zbufSockBufSend
    (
    int         s,          /* socket to send to */
    char *      buf,        /* pointer to data buffer */
    int         bufLen,     /* number of bytes to send */
    VOIDFUNCPTR freeRtn,    /* free routine callback */
    int         freeArg,    /* argument to free routine */
    int         flags       /* flags to underlying protocols */
    )
{
    return zbufSockBufSendto(s, buf, bufLen, freeRtn, freeArg, flags, NULL, 0);
}

/*******************************************************************************
*
* zbufSockBufSendto - create a zbuf from a user message and send it to a socket
*
* This routine creates a zbuf from the user buffer <buf>, and sends
* it to the socket named by <to>.  The socket <s> is the
* sending socket.
*
* The user-provided free routine callback at <freeRtn> is called when <buf>
* is no longer in use by the network stack.  Applications can
* exploit this callback to receive notification that <buf> is free.
* If <freeRtn> is NULL, the routine functions normally, except that the
* application has no way of being notified when <buf> is released by the
* network stack.  The free routine runs in the context of the task that last
* references the buffer.  This is typically either tNetTask context,
* or the caller's task context.  Declare <freeRtn> as follows
* (using whatever name is convenient):
* .CS
*       void freeCallback
*           (
*           caddr_t     buf,    /@ pointer to user buffer @/
*           int         freeArg /@ user-provided argument to free routine @/
*           )
* .CE
*
* You may OR the following values into the <flags> parameter with this
* operation:
*
* .iP "MSG_OOB (0x1)" 26
* Out-of-band data.
*
* .iP "MSG_DONTROUTE (0x4)"
* Send without using routing tables.
* .LP
*
* PROTECTION DOMAINS
* This function can only be called from within the kernel protection domain.
*
* RETURNS:
* The number of bytes sent, or ERROR if the call fails.
*
* SEE ALSO
* zbufSockSendto(), sendto()
*/

int zbufSockBufSendto
    (
    int         s,          /* socket to send to */
    char *      buf,        /* pointer to data buffer */
    int         bufLen,     /* number of bytes to send */
    VOIDFUNCPTR freeRtn,    /* free routine callback */
    int         freeArg,    /* argument to free routine */
    int         flags,      /* flags to underlying protocols */
    struct sockaddr *to,    /* recipient's address */
    int         tolen       /* length of <to> socket addr */
    )
{
    ZBUF_ID zbufId = zbufCreate ();
    zbufInsertBuf (zbufId, NULL, 0, buf, bufLen, freeRtn, freeArg);
    return zbufSockSendto(s, zbufId, bufLen, flags, to, tolen);
}

/*******************************************************************************
*
* zbufSockRecv - receive data in a zbuf from a socket
*
* This routine receives data from a connected socket, and
* returns the data to the user in a newly created zbuf.
*
* The <pLen> parameter indicates the number of bytes requested by the caller.
* If the operation is successful, the number of bytes received is
* copied to <pLen>.
*
* You may OR the following values into the <flags> parameter with this
* operation:
*
* .iP "MSG_OOB (0x1)" 26
* Out-of-band data.
*
* .iP "MSG_PEEK (0x2)"
* Return data without removing it from socket.
* .LP
*
* Once the user application is finished with the zbuf, zbufDelete() should
* be called to return the zbuf memory buffer to the VxWorks network stack.
*
* PROTECTION DOMAINS
* This function can only be called from within the kernel protection domain.
*
* RETURNS:
* The zbuf ID of a newly created zbuf containing the received data,
* or NULL if the operation fails.
*
* SEE ALSO
* recv()
*/

ZBUF_ID zbufSockRecv
    (
    int         s,          /* socket to receive data from */
    int         flags,      /* flags to underlying protocols */
    int *       pLen        /* number of bytes requested/returned */
    )
{
    return zbufSockRecvfrom(s, flags, pLen, NULL, NULL);
}

/*******************************************************************************
* Used by zbufSockRecvfrom
*/
static void
simple_free(void* ptr, int dummy)
{
    (void)dummy;
    ipcom_free(ptr);
}

/*******************************************************************************
*
* zbufSockRecvfrom - receive a message in a zbuf from a socket
*
* This routine receives a message from a socket, and
* returns the message to the user in a newly created zbuf.
*
* The message is received regardless of whether the socket is connected.
* If <from> is nonzero, the address of the sender's socket is copied to it.
* Initialize the value-result parameter <pFromLen> to the size of
* the <from> buffer.  On return, <pFromLen> contains the actual size of the
* address stored in <from>.
*
* The <pLen> parameter indicates the number of bytes requested by the caller.
* If the operation is successful, the number of bytes received is
* copied to <pLen>.
*
* You may OR the following values into the <flags> parameter with this
* operation:
*
* .iP "MSG_OOB (0x1)" 26
* Out-of-band data.
*
* .iP "MSG_PEEK (0x2)"
* Return data without removing it from socket.
* .LP
*
* Once the user application is finished with the zbuf, zbufDelete() should
* be called to return the zbuf memory buffer to the VxWorks network stack.
*
* PROTECTION DOMAINS
* This function can only be called from within the kernel protection domain.
*
* RETURNS:
* The zbuf ID of a newly created zbuf containing the received message,
* or NULL if the operation fails.
*/

ZBUF_ID zbufSockRecvfrom
    (
    int              s,         /* socket to receive from */
    int              flags,     /* flags to underlying protocols */
    int *            pLen,      /* number of bytes requested/returned */
    struct sockaddr *from,      /* where to copy sender's addr */
    int *            pFromLen   /* value/result length of <from> */
    )
{
    ZBUF_ID     zbufId;         /* zbuf returned to user */
    ZBUF_SEG    zbufSeg = NULL; /* received zbuf chain */
    int         status;         /* recvfrom() return status */
    char*       buf = NULL;
    int         so_type;
    socklen_t   optlen = sizeof(int);

    if (-1 == getsockopt(s, (int)SOL_SOCKET, (int)SO_TYPE, (char*)&so_type, (int*)&optlen))
        return (ZBUF_ID)NULL;

    ZBUF_ID_CREATE(zbufId);     /* create ID for recv data */
    if (zbufId == (ZBUF_ID)NULL)
        return (ZBUF_ID)NULL;

    if (so_type == SOCK_DGRAM)
    {
        if((status = recvfrom(s, (char*)&zbufSeg, *pLen, flags | IP_MSG_ZBUF, from, pFromLen)) < 0)
            goto cleanup;
    }
    else
    {
        ip_assert(so_type == SOCK_STREAM);
        buf = ipcom_malloc(*pLen);
        if (buf == NULL)
            goto cleanup;
        if ((status = recvfrom(s, buf, *pLen, flags | IP_MSG_ZBUF, from, pFromLen)) < 0)
            goto cleanup;
        if ((zbufSeg = zbufInsertBuf(zbufId, NULL, 0, buf, status, simple_free, 0)) == NULL)
            goto cleanup;
    }

    ZBUF_SETSEG(zbufId, zbufSeg);
    *pLen = status;
    return zbufId;

cleanup:
    if (zbufId)
        ZBUF_ID_DELETE_EMPTY(zbufId);
    if (buf)
        ipcom_free(buf);
    return (ZBUF_ID)NULL;
}

#else
int ipcom_zbufSockLib_empty_file;
#endif /* IPCOM_ZEROCOPY */

