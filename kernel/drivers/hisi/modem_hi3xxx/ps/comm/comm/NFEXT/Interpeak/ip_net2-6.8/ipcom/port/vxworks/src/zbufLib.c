/* zbufLib.c - zbuf interface library */

/* Copyright 1984 - 2005,2008 Wind River Systems, Inc. */

#include "ipcom_config.h"

#ifdef IPCOM_ZEROCOPY

/*
modification history
--------------------
01g,28jan08,aes  complete revamp. Using Ipcom_pkt instead of mbuf.
01f,12jan05,vvv  osdep.h cleanup
01e,02jul04,vvv  fixed warning
01d,05nov03,rlm  Ran batch header update for header re-org.
01c,26jan03,nee  Fixing compilation errors
01b,26jan03,nee  Checking for valid virtual stack context
01a,19nov02,nee  Ported to Accordion from "zbufLib.c@@/main/tor3_x/tor3_x.synth/1".
*/

/*
DESCRIPTION
This library contains routines to create, build, manipulate, and
delete zbufs.  Zbufs, also known as "zero copy buffers," are a data
abstraction designed to allow software modules to share buffers
without unnecessarily copying data.

To support the data abstraction, the subroutines in this library hide
the implementation details of zbufs.  This also maintains the
library's independence from any particular implementation mechanism,
permitting the zbuf interface to be used with other buffering schemes
eventually.

Zbufs have three essential properties.  First, a zbuf holds a
sequence of bytes.  Second, these bytes are organized into one or more
segments of contiguous data, although the successive segments
themselves are not usually contiguous.  Third, the data within a
segment may be shared with other segments; that is, the data may be in use
by more than one zbuf at a time.

ZBUF TYPES
The following data types are used in managing zbufs:
.iP ZBUF_ID 16 3
An arbitrary (but unique) integer that identifies a particular zbuf.
.iP ZBUF_SEG
An arbitrary (but unique within a single zbuf) integer that identifies
a segment within a zbuf.
.LP

ADDRESSING BYTES IN ZBUFS
The bytes in a zbuf are addressed by the combination <zbufSeg>,
<offset>.  The <offset> may be positive or negative, and is simply the
number of bytes from the beginning of the segment <zbufSeg>.

A <zbufSeg> can be specified as NULL, to identify the segment at the
beginning of a zbuf.  If <zbufseg> is NULL, <offset> is the
absolute offset to any byte in the zbuf.  However, it is more
efficient to identify a zbuf byte location relative to the <zbufSeg>
that contains it; see zbufSegFind() to convert any <zbufSeg>, <offset>
pair to the most efficient equivalent.

Negative <offset> values always refer to bytes before the
corresponding <zbufSeg>, and are usually not the most efficient
address formulation in themselves (though using them may save your
program other work in some cases).

The following special <offset> values, defined as constants,
allow you to specify the very beginning or the very end of an entire zbuf,
regardless of the <zbufSeg> value:
.iP ZBUF_BEGIN 16 3
The beginning of the entire zbuf.
.iP ZBUF_END
The end of the entire zbuf (useful for appending to a zbuf; see below).
.LP

INSERTION AND LIMITS ON OFFSETS
An <offset> is not valid if it points outside the zbuf.  Thus, to
address data currently within an N-byte zbuf, the valid offsets
relative to the first segment are 0 through N-1.

Insertion routines are a special case: they obey the usual
convention, but they use <offset> to specify where the new data
begins after the insertion is complete.  With regard to the original
zbuf data, therefore, data is always inserted just before the byte
location addressed by the <offset> value.  The value of this
convention is that it permits inserting (or concatenating) data either
before or after the existing data.  To insert before all the data
currently in a zbuf segment, use 0 as <offset>.  To insert after all
the data in an N-byte segment, use N as <offset>. An <offset> of N-1
inserts the data just before the last byte in an N-byte segment.

An <offset> of 0 is always a valid insertion point; for an empty zbuf,
0 is the only valid <offset> (and NULL the only valid <zbufSeg>).

SHARING DATA
The routines in this library avoid copying segment data whenever
possible.  Thus, by passing and manipulating ZBUF_IDs rather than
copying data, multiple programs can communicate with greater
efficiency.  However, each program must be aware of data sharing:
changes to the data in a zbuf segment are visible to all zbuf segments
that reference the data.

To alter your own program's view of zbuf data without affecting other
programs, first use zbufDup() to make a new zbuf; then you can use an
insertion or deletion routine, such as zbufInsertBuf(), to add a
segment that only your program sees (until you pass a zbuf containing
it to another program).  It is safest to do all direct data
manipulation in a private buffer, before enrolling it in a zbuf: in
principle, you should regard all zbuf segment data as shared.

Once a data buffer is enrolled in a zbuf segment, the zbuf library is
responsible for noticing when the buffer is no longer in use by any
program, and freeing it.  To support this, zbufInsertBuf() requires
that you specify a callback to a free routine each time you build a
zbuf segment around an existing buffer.  You can use this callback to notify
your application when a data buffer is no longer in use.

PROTECTION DOMAINS
This feature can only be used from within the kernel protection domain.

To use this feature, include the following component:
INCLUDE_ZBUF_SOCK

SEE ALSO
zbufSockLib,
.I "VxWorks AE Network Programmer's Guide: Networking APIs"

INTERNAL

Extensions:

zbufJoin (zbufId, zbufSeg) - coalesce two adjacent segment fragments
    This routine combines two or more contiguous zbuf segments into a single
    zbuf segment.  Such an operation is only feasible for joining segments
    that have the same freeRtn and freeArg, and that follow eachother in
    the zbuf.  This could be useful for coalescing zbuf segments fragmented
    by split operations.

zbufOwn (zbufId, zbufSeg, offset, len) - own a local copy of zbuf data
    This routine makes a local copy of a portion of a zbuf.  This
    is useful if you want to change the actual data in a zbuf, but want to
    make sure that you are not corrupting another zbuf that shares the
    same data.  If you are the only zbuf segment using a particular buffer,
    this routine does nothing.  If other zbufs reference the same data, this
    routine inserts a copy of the requested data, then deletes reference
    to the original data.
*/

/* includes */

#include <vxWorks.h>
#include <errnoLib.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#ifdef VIRTUAL_STACK
#include <netinet/vsLib.h>
#endif  /* VIRTUAL_STACK */

#include <ipcom_pkt.h>
#include <ipcom_netif.h>
#include <ipcom_os.h>
#include <ipcom_clib.h>
#include <ipnet_h.h>

#include <zbufLib.h>
#include "zbufLib_h.h"


/* locals */
typedef struct ZBUF_DATA_STRUCT
{
    Ipcom_pkt*  pkt;
    int         used;
}
ZBUF_DATA;

LOCAL int                           _num_zbuf;
LOCAL ZBUF_DATA                     *_zbuf_data;
LOCAL Ipcom_mutex                   _zbuf_mutex;

/* defines */

#define ZBUF_LOCK()                 ipcom_mutex_lock(_zbuf_mutex)
#define ZBUF_UNLOCK()               ipcom_mutex_unlock(_zbuf_mutex)

#define NUM_ZBUF                    100     /* Number of ZBUF_DATA to grow the buffer with each time */

#define ZSEG_START(pkt)             ( ((Ipcom_pkt *)pkt)->start)
#define ZSEG_MAXLEN(pkt)            ( ((Ipcom_pkt *)pkt)->maxlen)
#define ZSEG_LEN(pkt)               ( ((Ipcom_pkt *)pkt)->end - ((Ipcom_pkt *)pkt)->start)
#define ZSEG_CLEAN_SETLEN(pkt, len) { Ipcom_pkt *p = pkt; int _len = p->end - p->start; p->start -= (len -_len); }
#define ZSEG_SHRINK_SETLEN(pkt, len){ Ipcom_pkt *p = pkt; int _len = p->end - p->start; p->end += (len -_len); }

#define CREATE_SEG(pkt)             {                                       \
    pkt = ipcom_pkt_malloc(1500, IP_FLAG_FC_BLOCKOK);                       \
    if (pkt != IP_NULL)                                                     \
    {   pkt->end = pkt->start = pkt->maxlen;  }  }

#define CREATE_EMPTY_SEG(pkt)              {                                \
    pkt = ipcom_pkt_malloc(0, IP_FLAG_FC_BLOCKOK); }


#define ztod(pkt, type)             (type) &( ((Ipcom_pkt *)pkt)->data[((Ipcom_pkt *)pkt)->start])


/* prototypes and functions */
LOCAL ZBUF_SEG *    _zbufSegFindPrev(ZBUF_ID zbufId, ZBUF_SEG zbufSeg, int *pOffset);

LOCAL Ipcom_pkt *
_zsegFreeReturnNext(Ipcom_pkt * pkt)
{
    if ( pkt )
    {
        Ipcom_pkt *next = ZSEG_NEXT(pkt);
        ZSEG_NEXT(pkt) = 0;
        ipcom_pkt_free(pkt);
        return next;
    }
    return NULL;
}

void
_zbufDeleteEmpty(int index)
{
    ZBUF_LOCK();
    _zbuf_data[index].used = 0;
    _zbuf_data[index].pkt = 0;
    ZBUF_UNLOCK();
}

ZBUF_SEG*
_zbufGetSeg(int index)
{
    ZBUF_SEG* seg;
    ZBUF_LOCK();
    seg = &_zbuf_data[index].pkt;
    ZBUF_UNLOCK();
    return seg;
}

void
_zbufSetSeg(int index, ZBUF_SEG seg)
{
    ZBUF_LOCK();
    _zbuf_data[index].pkt = seg;
    ZBUF_UNLOCK();
}

void
zbuf_free(void* ptr, int dummy)
{
    (void)dummy;
    /*
     * The application did not specify a free routine;
     * do not take it upon ourselves to free the buffer.
     * The application may be using buffers in round-robin
     * fashion, and counts on having enough buffers not to
     * reuse one before the stack is done with it.
     */
    /* ipcom_free(ptr); */
}

/*******************************************************************************
*
* zbufLibInit - initialize the zbuf interface library
*
* This routine initializes the zbuf interface facility.  It
* must be called before any zbuf routines are used.  This routine is
* typically called during the initialization of a VxWorks facility which
* uses or extends the zbuf interface, although this is not a requirement.
* The zbuf library may be used on its own, provided an appropriate back-end
* buffer mechanism is available, and is specified by <libInitRtn>.
*
* RETURNS:
* OK, or ERROR if the zbuf interface cannot be initialized.
*
* NOMANUAL
*/

STATUS
zbufLibInit ( FUNCPTR libInitRtn )  /* back-end buffer mechanism init rtn */
{
    (void)libInitRtn;

    _zbuf_data = (ZBUF_DATA*)ipcom_calloc(sizeof(ZBUF_DATA), NUM_ZBUF);
    if (NULL == _zbuf_data)
        return ERROR;
    _num_zbuf = NUM_ZBUF;

    if (ipcom_mutex_create(&_zbuf_mutex) != IPCOM_SUCCESS)
    {
        ipcom_free(_zbuf_data);
        return ERROR;
    }

    return OK;
}


/*******************************************************************************
*
* zbufCreate - create an empty zbuf
*
* This routine creates a zbuf, which remains empty (that is, it contains
* no data) until segments are added by the zbuf insertion routines.
* Operations performed on zbufs require a zbuf ID, which is returned by
* this routine.
*
* PROTECTION DOMAINS
* This function can only be called from within the kernel protection domain.
*
* RETURNS:
* A zbuf ID, or NULL if a zbuf cannot be created.
*
* SEE ALSO: zbufDelete()
*/

ZBUF_ID
zbufCreate()
{
    int index;
    ZBUF_DATA* temp = 0;

    ZBUF_LOCK();
    for ( index = 1;index < _num_zbuf;++index )
    {
        if ( !_zbuf_data[index].used )
            break;
    }
    if ( index == _num_zbuf )
    {
        temp = (ZBUF_DATA*)ipcom_malloc( sizeof(ZBUF_DATA) * (_num_zbuf + NUM_ZBUF) ); /* realloc */
        if (NULL == temp)
        {
            ZBUF_UNLOCK();
            errnoSet(ENOMEM);   /* Out of memory */
            return (ZBUF_ID)NULL;
        }
        ipcom_memset(temp, 0, sizeof(ZBUF_DATA) * (_num_zbuf + NUM_ZBUF) );
        ipcom_memcpy(temp, _zbuf_data, sizeof(ZBUF_DATA) * _num_zbuf);
        ipcom_free(_zbuf_data);
        _zbuf_data = temp;
        _num_zbuf += NUM_ZBUF;
    }
    _zbuf_data[index].used = 1;
    _zbuf_data[index].pkt = 0;
    ZBUF_UNLOCK();

    return index;   /* return new ID */
}

/*******************************************************************************
*
* zbufDelete - delete a zbuf
*
* This routine deletes any zbuf segments in the specified zbuf, then
* deletes the zbuf ID itself.  <zbufId> must not be used after this routine
* executes successfully.
*
* For any data buffers that were not in use by any other zbuf, zbufDelete()
* calls the associated free routine (callback).
*
* PROTECTION DOMAINS
* This function can only be called from within the kernel protection domain.
*
* RETURNS:
* OK, or ERROR if the zbuf cannot be deleted.
*
* SEE ALSO: zbufCreate(), zbufInsertBuf()
*/
STATUS
zbufDelete( ZBUF_ID zbufId )        /* zbuf to be deleted */
{
    ZBUF_LOCK();
    if ( zbufId > 0 && zbufId < _num_zbuf )
    {
        if ( _zbuf_data[zbufId].used)
        {
            ZBUF_SEG pkt = _zbuf_data[zbufId].pkt;
            while ( (pkt = _zsegFreeReturnNext(pkt)) != NULL )
                ;
            _zbuf_data[zbufId].pkt = 0;
            _zbuf_data[zbufId].used = 0;
            ZBUF_UNLOCK();
            return OK;
        }
    }
    ZBUF_UNLOCK();
    return ERROR;
}

/*******************************************************************************
*
* zbufInsert - insert a zbuf into another zbuf
*
* This routine inserts all <zbufId2> zbuf segments into <zbufId1> at the
* specified byte location.
*
* The location of insertion is specified by <zbufSeg> and <offset>.  See
* the zbufLib manual page for more information on specifying
* a byte location within a zbuf.  In particular, insertion within
* a zbuf occurs before the byte location specified by <zbufSeg> and <offset>.
* Additionally, <zbufSeg> and <offset> must be NULL and 0,
* respectively, when inserting into an empty zbuf.
*
* After all the <zbufId2> segments are inserted into <zbufId1>, the zbuf ID
* <zbufId2> is deleted.  <zbufId2> must not be used after this routine
* executes successfully.
*
* PROTECTION DOMAINS
* This function can only be called from within the kernel protection domain.
* All arguments MUST be valid in the kernel protection domain.  Returned value
* is valid in the protection domain only.
*
* ARGUMENTS:
* zbufId1       zbuf to insert <zbufId2> into
* zbufSeg       zbuf segment base for <offset>
* offset        relative byte offset
* zbufId2       zbuf to insert into <zbufId1>
*
* RETURNS:
* The zbuf segment ID for the first inserted segment,
* or NULL if the operation fails.
*/


ZBUF_SEG
zbufInsert( ZBUF_ID zbufId1, ZBUF_SEG zbufSeg,int offset, ZBUF_ID zbufId2 )
{

    ZBUF_ID     zbufIdNew;      /* dummy ID for dup operation */
    ZBUF_SEG *  pZbufPrev;      /* zbuf prev to insert */
    ZBUF_SEG    zbufEnd;        /* last Id2 zbuf */
    int         maxLen = ZBUF_END;  /* offset for last Id2 byte */

    /* find the zbuf ptr previous to the point of insertion */
    if ((pZbufPrev = _zbufSegFindPrev(zbufId1, zbufSeg, &offset)) == NULL)
        return NULL;

    if ((zbufEnd = zbufSegFind (zbufId2, NULL, &maxLen)) == NULL)
        return NULL;                /* find end zbuf of Id2 */

    if (offset == 0)                /* if prepend... */
    {
        ZSEG_NEXT(zbufEnd) = *pZbufPrev;
        *pZbufPrev = ZBUF_GETSEG(zbufId2);
    }
    else                    /* else insert... */
    {
        zbufSeg = *pZbufPrev;           /* find insertion zbuf */

        if ( (zbufIdNew = zbufDup( zbufId1, zbufSeg, offset, (ZSEG_LEN(zbufSeg) - offset))) == (ZBUF_ID)NULL)
            return NULL;            /* dup later portion */

        ZSEG_SHRINK_SETLEN(zbufSeg, offset);       /* shorten to first portion */
        ZSEG_NEXT(ZBUF_GETSEG(zbufIdNew)) = ZSEG_NEXT(zbufSeg);
        ZSEG_NEXT(zbufEnd) = ZBUF_GETSEG(zbufIdNew);    /* insert Id2 */
        ZSEG_NEXT(zbufSeg) = ZBUF_GETSEG(zbufId2);

        ZBUF_ID_DELETE_EMPTY(zbufIdNew);    /* delete dup ID */
    }

    zbufEnd = ZBUF_GETSEG(zbufId2);     /* save head for return */

    ZBUF_ID_DELETE_EMPTY(zbufId2);

    return zbufEnd;             /* return inserted zbuf */

}

/*******************************************************************************
*
* zbufInsertBuf - create a zbuf segment from a buffer and insert into a zbuf
*
* This routine creates a zbuf segment from the application buffer <buf>
* and inserts it at the specified byte location in <zbufId>.
*
* The location of insertion is specified by <zbufSeg> and <offset>.  See
* the zbufLib manual page for more information on specifying
* a byte location within a zbuf.  In particular, insertion within
* a zbuf occurs before the byte location specified by <zbufSeg> and <offset>.
* Additionally, <zbufSeg> and <offset> must be NULL and 0,
* respectively, when inserting into an empty zbuf.
*
* The parameter <freeRtn> specifies a free-routine callback that runs when
* the data buffer <buf> is no longer referenced by any zbuf segments.  If
* <freeRtn> is NULL, the zbuf functions normally, except that the
* application is not notified when no more zbufs segments reference <buf>.
* The free-routine callback runs from the context of the task that last
* deletes reference to the buffer.  Declare the <freeRtn> callback as
* follows (using whatever routine name suits your application):
* .CS
*   void freeCallback
*       (
*       caddr_t buf,    /@ pointer to application buffer @/
*       int     freeArg /@ argument to free routine @/
*       )
* .CE
*
* PROTECTION DOMAINS
* This function can only be called from within the kernel protection domain.
* All arguments MUST be valid in the kernel protection domain.  Returned value
* is valid in the protection domain only.
*
* RETURNS:
* The zbuf segment ID of the inserted segment,
* or NULL if the operation fails.
*/


ZBUF_SEG
zbufInsertBuf(
    ZBUF_ID     zbufId,     /* zbuf in which buffer is inserted */
    ZBUF_SEG    zbufSeg,    /* zbuf segment base for <offset> */
    int         offset,     /* relative byte offset */
    caddr_t     buf,        /* application buffer for segment */
    int         len,        /* number of bytes to insert */
    VOIDFUNCPTR freeRtn,    /* free-routine callback */
    int         freeArg     /* argument to free routine */
    )
{
    int zbufIdNew = 0;
    Ipcom_pkt* pkt = 0;
    Ipcom_pkt* first_pkt = 0;
    if (len <= 0)               /* have to insert some bytes */
    {
        errnoSet(S_zbufLib_LENGTH_INVALID);
        return NULL;
    }

    if ( offset == 0 )
    {
        CREATE_EMPTY_SEG(pkt);
        if (freeRtn == IP_NULL)
            ipcom_pkt_create(pkt, buf, len, (Ipcom_pkt_freefunc)zbuf_free, buf);
        else
            ipcom_pkt_create(pkt, buf, len, (Ipcom_pkt_freefunc)freeRtn, (void*)freeArg);

        pkt->start = 0;
        pkt->end = len;

        first_pkt = ZBUF_GETSEG(zbufId);
        if ( zbufSeg == NULL )
        {
            ZBUF_SETSEG(zbufId, pkt);
            if ( first_pkt != NULL )  /* insert first in a buf? */
                ZSEG_NEXT(pkt) = first_pkt;
        }
        else    /* insert before zbufSeg */
        {
            if ( first_pkt == IP_NULL ) /* the zbufSeg sent in isn't in this zbuf */
            {
                errnoSet(S_zbufLib_SEGMENT_NOT_FOUND);
                return(NULL);
            }
            if (zbufSeg == first_pkt)   /* insert before first */
            {
                ZBUF_SETSEG(zbufId, pkt);
                ZSEG_NEXT(pkt) = first_pkt;
            }
            else
            {
                Ipcom_pkt* pkt_prev = zbufSegPrev(zbufId, zbufSeg );
                ZSEG_NEXT(pkt) = zbufSeg;
                ZSEG_NEXT(pkt_prev) = pkt;
            }
        }
        return pkt;
    }

    ZBUF_ID_CREATE(zbufIdNew);
    if (zbufIdNew == 0)
        return NULL;

    CREATE_SEG(pkt);
    if (pkt == IP_NULL)
        return NULL;

    pkt->start -= len;
    /* align the start to 4 byte boundry */
    while ( (Ip_ptrdiff_t)(&(pkt->data[pkt->start])) & 0x3 )
    {
        pkt->start--;
        pkt->end--;
    }
    ipcom_memcpy( &pkt->data[pkt->start], buf, len );

    if ( freeRtn)
        (*freeRtn) (buf, freeArg);

    ZBUF_SETSEG(zbufIdNew, pkt);

    return zbufInsert( zbufId, zbufSeg,offset, zbufIdNew );
}

/*******************************************************************************
*
* zbufInsertCopy - copy buffer data into a zbuf
*
* This routine copies <len> bytes of data from the application buffer <buf>
* and inserts it at the specified byte location in <zbufId>.  The
* application buffer is in no way tied to the zbuf after this operation;
* a separate copy of the data is made.
*
* The location of insertion is specified by <zbufSeg> and <offset>.  See
* the zbufLib manual page for more information on specifying
* a byte location within a zbuf.  In particular, insertion within
* a zbuf occurs before the byte location specified by <zbufSeg> and <offset>.
* Additionally, <zbufSeg> and <offset> must be NULL and 0,
* respectively, when inserting into an empty zbuf.
*
* PROTECTION DOMAINS
* This function can only be called from within the kernel protection domain.
* All arguments MUST be valid in the kernel protection domain.  Returned value
* is valid in the protection domain only.
*
* RETURNS:
* The zbuf segment ID of the first inserted segment,
* or NULL if the operation fails.
*/

ZBUF_SEG
zbufInsertCopy(
    ZBUF_ID     zbufId,     /* zbuf into which data is copied */
    ZBUF_SEG    zbufSeg,    /* zbuf segment base for <offset> */
    int         offset,     /* relative byte offset */
    caddr_t     buf,        /* buffer from which data is copied */
    int         len         /* number of bytes to copy */
    )
{

    ZBUF_ID     zbufIdNew;      /* dummy ID for dup operation */
    ZBUF_SEG    zbufNew;        /* zbuf for copy */
    ZBUF_SEG *  pZbufPrev;      /* zbuf previous to zbufNew */
    int         length;         /* length of each copy */

    if ( len <= 0 )             /* have to copy some bytes */
    {
        errnoSet(S_zbufLib_LENGTH_INVALID);
        return(NULL);
    }

    ZBUF_ID_CREATE (zbufIdNew);         /* create new ID for copy */
    if ( zbufIdNew == (ZBUF_ID)NULL )
        return NULL;

    pZbufPrev = &ZBUF_GETSEG(zbufIdNew);       /* init prev ptr to head */

    while ( len )               /* while more to copy */
    {
        /* obtain a new zbuf with a data buffer pointed */
        CREATE_SEG(zbufNew);
        if ( zbufNew == NULL )
        {
            /* release on fail */
            ZBUF_ID_DELETE(zbufIdNew);
            return(NULL);
        }

        length = min(len, ZSEG_MAXLEN(zbufNew) ); /* num for copy */


        ZSEG_CLEAN_SETLEN(zbufNew, length); /* set len to num to copy.. (to make room) */
        bcopy(buf, ztod(zbufNew, char *), length);

        buf += length;              /* bump to new buf position */
        len -= length;

        *pZbufPrev = zbufNew;               /* hook prev into new */
        pZbufPrev = &ZSEG_NEXT(zbufNew);    /* update prev zbuf ptr */
    }

    /* insert the new zbuf ID with copied data into <zbufId> */
    if ( (zbufSeg = zbufInsert(zbufId, zbufSeg, offset, zbufIdNew)) == NULL )
    {
        /* release on fail */
        ZBUF_ID_DELETE(zbufIdNew);
        return NULL;
    }

    return zbufSeg;                /* return inserted zbuf */
}

/*******************************************************************************
*
* zbufExtractCopy - copy data from a zbuf to a buffer
*
* This routine copies <len> bytes of data from <zbufId> to the application
* buffer <buf>.
*
* The starting location of the copy is specified by <zbufSeg> and <offset>.
* See the zbufLib manual page for more information on
* specifying a byte location within a zbuf.  In particular, the first
* byte copied is the exact byte specified by <zbufSeg> and <offset>.
*
* The number of bytes to copy is given by <len>.  If this parameter
* is negative, or is larger than the number of bytes in the zbuf after the
* specified byte location, the rest of the zbuf is copied.
* The bytes copied may span more than one segment.
*
* PROTECTION DOMAINS
* This function can only be called from within the kernel protection domain.
* All arguments MUST be valid in the kernel protection domain.
*
* RETURNS:
* The number of bytes copied from the zbuf to the buffer,
* or ERROR if the operation fails.
*/

int
zbufExtractCopy(
    ZBUF_ID     zbufId,     /* zbuf from which data is copied */
    ZBUF_SEG    zbufSeg,    /* zbuf segment base for <offset> */
    int         offset,     /* relative byte offset */
    caddr_t     buf,        /* buffer into which data is copied */
    int         len     /* number of bytes to copy */
    )
{
    caddr_t     buf0 = buf;     /* save starting position */
    int         length;         /* length of each copy */

    /* find the starting location for copying */

    if ( (zbufSeg = zbufSegFind (zbufId, zbufSeg, &offset)) == NULL )
        return(ERROR);

    if ( len < 0 )              /* negative = rest of chain */
        len = INT_MAX;

    while ( len && (zbufSeg != NULL) )      /* while more to copy */
    {
        length = min(len, ( ZSEG_LEN(zbufSeg) - offset));  /* num for copy */

        bcopy (ztod(zbufSeg, char *) + offset, buf, length);

        buf += length;              /* bump to new buf position */
        len -= length;
        zbufSeg = ZSEG_NEXT(zbufSeg);  /* bump to next zbuf in chain */
        offset = 0;         /* no more offset */
    }

    return((int) buf - (int) buf0);         /* return num bytes copied */
}

/*******************************************************************************
*
* zbufCut - delete bytes from a zbuf
*
* This routine deletes <len> bytes from <zbufId> starting at the specified
* byte location.
*
* The starting location of deletion is specified by <zbufSeg> and <offset>.
* See the zbufLib manual page for more information on
* specifying a byte location within a zbuf.  In particular, the first
* byte deleted is the exact byte specified by <zbufSeg> and <offset>.
*
* The number of bytes to delete is given by <len>.  If this parameter
* is negative, or is larger than the number of bytes in the zbuf after the
* specified byte location, the rest of the zbuf is deleted.
* The bytes deleted may span more than one segment.
*
* If all the bytes in any one segment are deleted, then the segment is
* deleted, and the data buffer that it referenced will be freed if no
* other zbuf segments reference it.  No segment may survive with zero bytes
* referenced.
*
* Deleting bytes out of the middle of a segment splits the segment
* into two.  The first segment contains the portion of the data buffer
* before the deleted bytes, while the other segment contains the end
* portion that remains after deleting <len> bytes.
*
* This routine returns the zbuf segment ID of the segment just
* after the deleted bytes.  In the case where bytes are cut off the end
* of a zbuf, a value of ZBUF_NONE is returned.
*
* PROTECTION DOMAINS
* This function can only be called from within the kernel protection domain.
* All arguments MUST be valid in the kernel protection domain.  Returned value
* is valid in the protection domain only.
*
* RETURNS:
* The zbuf segment ID of the segment following the deleted bytes,
* or NULL if the operation fails.
*/

ZBUF_SEG
zbufCut(
    ZBUF_ID     zbufId,     /* zbuf from which bytes are cut */
    ZBUF_SEG    zbufSeg,    /* zbuf segment base for <offset> */
    int         offset,     /* relative byte offset */
    int         len     /* number of bytes to cut */
    )
{
    ZBUF_ID     zbufIdNew;  /* dummy ID for dup operation */
    ZBUF_SEG *  pZbufPrev;  /* zbuf prev deleted zbuf */
    int         length;     /* length of each cut */

    /* find the zbuf ptr previous to the cut */
    if ( (pZbufPrev = _zbufSegFindPrev(zbufId, zbufSeg, &offset)) == NULL )
        return(NULL);

    if ( (zbufSeg = *pZbufPrev) == NULL )   /* find cut zbuf */
    {
        errnoSet(S_zbufLib_SEGMENT_NOT_FOUND);
        return(NULL);
    }

    if ( len < 0 )              /* negative = rest of chain */
        len = INT_MAX;

    while ( len && (zbufSeg != NULL) )      /* while more to cut */
    {
        length = min(len, (ZSEG_LEN(zbufSeg) - offset)); /* num for cut */
        len -= length;

        if ( offset != 0 )          /* if !cutting off front... */
        {
            if ( ZSEG_LEN(zbufSeg) != (offset + length) )/* cut from middle*/
            {
                /* duplicate portion remaining after bytes to be cut */

                if ( (zbufIdNew = zbufDup(zbufId, zbufSeg, offset + length,
                                          ZSEG_LEN(zbufSeg) - offset - length)) == (ZBUF_ID)NULL )
                    return(NULL);

                ZSEG_NEXT(ZBUF_GETSEG(zbufIdNew))   = ZSEG_NEXT(zbufSeg);
                ZSEG_NEXT(zbufSeg)                  = ZBUF_GETSEG(zbufIdNew);   /* hook in saved data */
                ZSEG_SHRINK_SETLEN(zbufSeg, offset);                            /* shorten to later portion */
                ZBUF_ID_DELETE_EMPTY(zbufIdNew);                                /* delete dup ID */
                return ZSEG_NEXT(zbufSeg);                                      /* return next real zbuf */
            }
            else                /* cut to end */
            {
                ZSEG_SHRINK_SETLEN(zbufSeg, ZSEG_LEN(zbufSeg) - length);    /* decrease by len deleted */
                pZbufPrev = &ZSEG_NEXT(zbufSeg);                            /* update previous */
                zbufSeg = ZSEG_NEXT(zbufSeg);                               /* bump current zbuf to next */
            }

            offset = 0;         /* no more offset */
        }
        else                    /* cutting off front... */
        {
            if ( length == ZSEG_LEN(zbufSeg) )   /* cutting whole zbuf ? */
            {
                zbufSeg = _zsegFreeReturnNext(zbufSeg); /* free and get next zbuf */
                *pZbufPrev = zbufSeg;       /* hook prev to next zbuf */
            }
            else                /* cut off front portion */
            {
                ZSEG_START(zbufSeg) += length;
            }
        }
    }

    if ( zbufSeg == NULL )          /* special case - cut off end */
        return(ZBUF_NONE);

    return(zbufSeg);                /* return next real zbuf */


}

/*******************************************************************************
*
* zbufSplit - split a zbuf into two separate zbufs
*
* This routine splits <zbufId> into two separate zbufs at the specified
* byte location.  The first portion remains in <zbufId>, while the end
* portion is returned in a newly created zbuf.
*
* The location of the split is specified by <zbufSeg> and <offset>.  See
* the zbufLib manual page for more information on specifying
* a byte location within a zbuf.  In particular, after the split
* operation, the first byte of the returned zbuf is the exact byte
* specified by <zbufSeg> and <offset>.
*
* PROTECTION DOMAINS
* This function can only be called from within the kernel protection domain.
* All arguments MUST be valid in the kernel protection domain.  Returned value
* is valid in the protection domain only.
*
* RETURNS:
* The zbuf ID of a newly created zbuf containing the end portion of <zbufId>,
* or NULL if the operation fails.
*/

ZBUF_ID
zbufSplit(
    ZBUF_ID     zbufId,     /* zbuf to split into two */
    ZBUF_SEG    zbufSeg,    /* zbuf segment base for <offset> */
    int         offset      /* relative byte offset */
    )
{
    ZBUF_ID     zbufIdNew;      /* zbuf ID of later portion */
    ZBUF_SEG *  pZbufPrev;      /* zbuf prev to insert */

    /* find the zbuf ptr previous to the split */
    if ( (pZbufPrev = _zbufSegFindPrev(zbufId, zbufSeg, &offset)) == NULL )
        return (ZBUF_ID)NULL;

    if ( offset == 0 )              /* in middle of zbuf */
    {
        ZBUF_ID_CREATE(zbufIdNew);     /* create ID for end portion */
        if ( zbufIdNew == (ZBUF_ID)NULL )
            return (ZBUF_ID)NULL;

        ZBUF_GETSEG(zbufIdNew) = *pZbufPrev;   /* hook in new head */
        *pZbufPrev = NULL;          /* tie off first portion */
    }
    else                    /* split on zbuf boundary */
    {
        zbufSeg = *pZbufPrev;           /* find split zbuf */
        if ( (zbufIdNew = zbufDup(zbufId, zbufSeg, offset, ZSEG_LEN(zbufSeg) - offset)) == (ZBUF_ID)NULL )
            return (ZBUF_ID)NULL;       /* dup later portion */

        ZSEG_NEXT(ZBUF_GETSEG(zbufIdNew)) = ZSEG_NEXT(zbufSeg); /* point the new buf foward.. */
        ZSEG_SHRINK_SETLEN(zbufSeg, offset);
        ZSEG_NEXT(zbufSeg) = NULL;         /* tie off first portion */
    }

    return zbufIdNew;              /* return ID for end */
}

/*******************************************************************************
*
* zbufDup - duplicate a zbuf
*
* This routine duplicates <len> bytes of <zbufId> starting at the specified
* byte location, and returns the zbuf ID of the newly created duplicate zbuf.
*
* The starting location of duplication is specified by <zbufSeg> and <offset>.
* See the zbufLib manual page for more information on
* specifying a byte location within a zbuf.  In particular, the first
* byte duplicated is the exact byte specified by <zbufSeg> and <offset>.
*
* The number of bytes to duplicate is given by <len>.  If this parameter
* is negative, or is larger than the number of bytes in the zbuf after the
* specified byte location, the rest of the zbuf is duplicated.
*
* Duplication of zbuf data does not usually involve copying of the data.
* Instead, the zbuf segment pointer information is duplicated, while the data
* is not, which means that the data is shared among all zbuf segments
* that reference the data.  See the zbufLib manual
* page for more information on copying and sharing zbuf data.
*
* RETURNS:
* The zbuf ID of a newly created duplicate zbuf,
* or NULL if the operation fails.
*/

ZBUF_ID
zbufDup(
    ZBUF_ID     zbufId,     /* zbuf to duplicate */
    ZBUF_SEG        zbufSeg,    /* zbuf segment base for <offset> */
    int         offset,     /* relative byte offset */
    int         len     /* number of bytes to duplicate */
    )
{

    ZBUF_ID         zbufIdNew;  /* zbuf ID of duplicate */
    ZBUF_SEG        zbufNew;    /* zbuf for duplicate */
    ZBUF_SEG *      pZbufPrev;  /* zbuf prev to zbufNew */

    /* find the starting location for duplicate */
    if ( (zbufSeg = zbufSegFind(zbufId, zbufSeg, &offset)) == NULL )
        return (ZBUF_ID)NULL;

    if ( len < 0 )               /* negative = rest of chain */
        len = INT_MAX;

    ZBUF_ID_CREATE(zbufIdNew);            /* get ID for duplicate */
    if ( zbufIdNew == (ZBUF_ID)NULL )
        return (ZBUF_ID)NULL;

    pZbufPrev = &(ZBUF_GETSEG(zbufIdNew));      /* init prev ptr to head */

    while ( len && (zbufSeg != NULL) )       /* while more to duplicate */
    {
        zbufNew = ipnet_pkt_clone(zbufSeg, IP_FALSE );

        zbufNew->start += offset;   /* if starting at an offset */
        len -= ZSEG_LEN(zbufNew);
        if ( len < 0 )              /* if copy to many bytes, shrink */
        {
            zbufNew->end += len;
            len = 0;
        }

        *pZbufPrev = zbufNew;          /* hook prev into duplicate */
        pZbufPrev = &ZSEG_NEXT(zbufNew);      /* update prev zbuf ptr */
        zbufSeg = ZSEG_NEXT(zbufSeg);     /* bump original chain */

        offset = 0;                /* no more offset */
    }

     return zbufIdNew;              /* return ID of duplicate */
}

/*******************************************************************************
*
* zbufLength - determine the length in bytes of a zbuf
*
* This routine returns the number of bytes in the zbuf <zbufId>.
*
* PROTECTION DOMAINS
* This function can only be called from within the kernel protection domain.
* All arguments MUST be valid in the kernel protection domain.
*
* RETURNS:
* The number of bytes in the zbuf,
* or ERROR if the operation fails.
*/

int
zbufLength(
    ZBUF_ID     zbufId      /* zbuf to determine length */
    )
{
    int len = 0;
    ZBUF_SEG seg = ZBUF_GETSEG(zbufId);
    while ( seg )
    {
        len += ZSEG_LEN(seg);
        seg = ZSEG_NEXT(seg);
    }
    return len;
}






/*******************************************************************************
*
* _zbufSegFindPrev - find the zbuf prev to a specified byte location
*
* This routine finds the zbuf in <zbufId> that is previous to the byte
* location specified by <zbufSeg> and <pOffset>.  Once found, a pointer
* to the m_next pointer of that zbuf is returned by this routine,
* and the offset to the specified location is returned in <pOffset>.
*
* This routine is similar to _zbufSegFind(), except that a pointer to the
* previous zbuf's m_next is returned by this routine, instead of the zbuf
* itself.  Additionally, the end boundary case differs.  This routine will
* return a valid pointer and offset when the specified byte is the byte just
* past the end of the chain.  Return values for this "imaginary" byte
* will also be returned when an offset of ZBUF_END is passed in.
*
* RETURNS:
* A pointer to the m_next pointer of the zbuf previous to the zbuf associated
* with the zbuf containing the specified byte, or NULL if the operation failed.
*
* NOMANUAL
*/

LOCAL ZBUF_SEG *
_zbufSegFindPrev(
        ZBUF_ID         zbufId,         /* zbuf ID to examine */
        ZBUF_SEG        zbufSeg,        /* zbuf base for <pOffset> */
        int *           pOffset         /* relative byte offset */
)
{
    ZBUF_SEG *      pZbufPrev;      /* prev ptr for return */
    ZBUF_SEG        zbuf;           /* zbuf in chain */
    int             offset;         /* offset in bytes */
    int             length;         /* length counter */

    if ( zbufId == (ZBUF_ID)NULL )
    {
        errnoSet(S_zbufLib_ID_INVALID);
        return NULL;
    }

    pZbufPrev = &ZBUF_GETSEG(zbufId);         /* init prev ptr to head */

    if ( (zbuf = ZBUF_GETSEG(zbufId)) == NULL )  /* is this chain empty ? */
    {
        if ( (zbufSeg == NULL) && (*pOffset == 0) )/* empty zbuf ID ? */
            return(pZbufPrev);      /* OK, if explicit */
        else                    /* else error condition */
        {
            errnoSet(S_zbufLib_ID_EMPTY);
            return NULL;
        }
    }

    if ( (offset = *pOffset) == ZBUF_BEGIN )  /* shortcut to head of chain */
    {
        *pOffset = 0;
        return pZbufPrev;
    }
    else if ( offset == ZBUF_END )      /* shortcut to end of chain */
    {
        if ( zbufSeg != NULL )
            zbuf = zbufSeg;     /* set base as <zbufSeg> */

        for ( ; ZSEG_NEXT(zbuf) != NULL; zbuf = ZSEG_NEXT(zbuf) )
            ;               /* find last zbuf in chain */

        *pOffset = 0;               /* imaginary byte past zbuf */
        return &ZSEG_NEXT(zbuf);
    }
    else if ( offset < 0 )          /* counting backwards ? */
    {
        if ( (zbufSeg == NULL) || (zbufSeg == ZBUF_GETSEG(zbufId)) )
        {
            errnoSet(S_zbufLib_OFFSET_INVALID);
            return NULL;       /* offset before head */
        }

        for ( length = 0; (ZSEG_NEXT(zbuf) != NULL) && (ZSEG_NEXT(zbuf) != zbufSeg);
            zbuf = ZSEG_NEXT(zbuf) )
        {
            length   += ZSEG_LEN(zbuf);  /* find length up to base */
            pZbufPrev = &ZSEG_NEXT(zbuf);
        }

        if ( ZSEG_NEXT(zbuf) == NULL )
        {
            errnoSet(S_zbufLib_SEGMENT_NOT_FOUND);
            return NULL;       /* couldn't find base zbufSeg */
        }

        if ( -(offset) < ZSEG_LEN(zbuf) )      /* within one zbuf */
        {
            *pOffset += ZSEG_LEN(zbuf);
            return pZbufPrev;
        }

        if ( (offset += length + ZSEG_LEN(zbuf)) < 0 )/* adjust to positive */
        {
            errnoSet(S_zbufLib_OFFSET_INVALID);
            return NULL;
        }

        zbuf = ZBUF_GETSEG(zbufId);
        pZbufPrev = &ZBUF_GETSEG(zbufId);      /* init prev ptr to head */
    }
    else if ( (zbufSeg != NULL) && (zbufSeg != zbuf) )/* new base, init head */
    {
        if ( offset < ZSEG_LEN(zbufSeg) )  /* dest within this base ? */
        {
            if ( (zbuf = zbufSegPrev (zbufId, zbufSeg)) != NULL )
                return(&ZSEG_NEXT(zbuf));  /* located previous zbuf */
            else
                return NULL;
        }

        zbuf = zbufSeg;         /* set base as <zbufSeg> */
    }

    for ( ; (zbuf != NULL) && (offset >= ZSEG_LEN(zbuf)); zbuf = ZSEG_NEXT(zbuf) )
    {
        offset   -= ZSEG_LEN(zbuf);          /* find right zbuf in chain */
        pZbufPrev = &ZSEG_NEXT(zbuf);
    }

    if ( (zbuf == NULL) && (offset != 0) )
    {
        errnoSet(S_zbufLib_OFFSET_INVALID);
        return NULL;           /* too large offset */
    }

    *pOffset = offset;              /* return new offset */
    return pZbufPrev;              /* return found zbuf */
}


/*******************************************************************************
*
* zbufSegFind - find the zbuf segment containing a specified byte location
*
* This routine translates an address within a zbuf to its most local
* formulation.  zbufSegFind() locates the zbuf segment in <zbufId>
* that contains the byte location specified by <zbufSeg> and *<pOffset>,
* then returns that zbuf segment, and writes in *<pOffset> the new offset
* relative to the returned segment.
*
* If the <zbufSeg>, *<pOffset> pair specify a byte location past the end
* of the zbuf, or before the first byte in the zbuf, zbufSegFind()
* returns NULL.
*
* See the zbufLib manual page for a full discussion of addressing zbufs
* by segment and offset.
*
* PROTECTION DOMAINS
* This function can only be called from within the kernel protection domain.
* All arguments MUST be valid in the kernel protection domain.  Returned value
* is valid in the protection domain only.
*
* RETURNS:
* The zbuf segment ID of the segment containing the specified byte,
* or NULL if the operation fails.
*/

ZBUF_SEG
zbufSegFind(
    ZBUF_ID     zbufId,     /* zbuf to examine */
    ZBUF_SEG    zbufSeg,    /* zbuf segment base for <pOffset> */
    int *       pOffset     /* relative byte offset */
    )
{
    ZBUF_SEG    zbuf;           /* zbuf in chain */
    ZBUF_SEG    zbufOrig;       /* first zbuf in chain */
    int         offset;         /* offset in bytes */
    int         length;         /* length counter */

    if ( zbufId == 0 )          /* invalid ID ? */
    {
        errnoSet(S_zbufLib_ID_INVALID);
        return(NULL);
    }

    zbuf = zbufOrig = ZBUF_GETSEG(zbufId);
    if ( zbuf == NULL )      /* is this chain empty ? */
    {
        errnoSet(S_zbufLib_ID_EMPTY);
        return(NULL);
    }

    if ( pOffset == NULL )  /* error.. passed in NULL pointer as offset */
    {
        errnoSet(S_zbufLib_OFFSET_INVALID);
        return NULL;
    }

    if ( (offset = *pOffset) == ZBUF_BEGIN )      /* shortcut to head of chain */
    {
        *pOffset = 0;
        return zbuf;
    }
    else if ( offset == ZBUF_END )      /* shortcut to end of chain */
    {
        if ( zbufSeg != NULL )
            zbuf = zbufSeg;     /* set base as <zbufSeg> */

        for ( ; ZSEG_NEXT(zbuf) != NULL; zbuf = ZSEG_NEXT(zbuf) )
            ;               /* find last zbuf in chain */

        *pOffset = ZSEG_LEN(zbuf) - 1;     /* stuff new offset */
        return zbuf;
    }
    else if ( offset < 0 )          /* counting backwards ? */
    {
        if ( (zbufSeg == NULL) || (zbufSeg == zbufOrig) )
        {
            errnoSet(S_zbufLib_OFFSET_INVALID);
            return NULL;       /* offset before head */
        }

        for ( length = 0; (ZSEG_NEXT(zbuf) != NULL) && (ZSEG_NEXT(zbuf) != zbufSeg);zbuf = ZSEG_NEXT(zbuf) )
            length += ZSEG_LEN(zbuf);          /* find length up to base */

        if ( ZSEG_NEXT(zbuf) == NULL )
        {
            errnoSet(S_zbufLib_SEGMENT_NOT_FOUND);
            return(NULL);       /* couldn't find base zbufSeg */
        }

        if ( -(offset) < ZSEG_LEN(zbuf) )      /* within one zbuf */
        {
            *pOffset += ZSEG_LEN(zbuf);
            return zbuf;
        }

        if ( (offset += length + ZSEG_LEN(zbuf)) < 0 )    /* adjust to positive */
        {
            errnoSet(S_zbufLib_OFFSET_INVALID);
            return NULL;
        }

        zbuf = zbufOrig;
    }
    else if ( zbufSeg != NULL )
        zbuf = zbufSeg;         /* set base as <zbufSeg> */

    for ( ; (zbuf != NULL) && (offset >= ZSEG_LEN(zbuf)); zbuf = ZSEG_NEXT(zbuf) )
        offset -= ZSEG_LEN(zbuf);          /* find right zbuf in chain */

    if ( zbuf == NULL )
    {
        errnoSet(S_zbufLib_OFFSET_INVALID);
    }
    else
        *pOffset = offset;          /* return new offset */

    return zbuf;               /* return found zbuf */
}

/*******************************************************************************
*
* zbufSegNext - get the next segment in a zbuf
*
* This routine finds the zbuf segment in <zbufId> that is just after
* the zbuf segment <zbufSeg>.  If <zbufSeg> is NULL, the segment after
* the first segment in <zbufId> is returned.  If <zbufSeg> is the last
* segment in <zbufId>, NULL is returned.
*
* RETURNS:
* The zbuf segment ID of the segment after <zbufSeg>,
* or NULL if the operation fails.
*/

ZBUF_SEG
zbufSegNext(
    ZBUF_ID         zbufId,     /* zbuf to examine */
    ZBUF_SEG        zbufSeg     /* segment to get next segment */
    )
{
    if ( zbufId == (ZBUF_ID)NULL )
    {
        errnoSet(S_zbufLib_ID_INVALID);
        return NULL;
    }

    if ( ZBUF_GETSEG(zbufId) == NULL )     /* is this chain empty ? */
    {
        errnoSet(S_zbufLib_ID_EMPTY);
        return NULL;
    }

    /* get zbuf after <zbufSeg> (or start of chain if <zbufSeg> is NULL) */
    if ( zbufSeg == NULL )
        return ZSEG_NEXT( ZBUF_GETSEG(zbufId) );
    else
        return ZSEG_NEXT(zbufSeg);
}

/*******************************************************************************
*
* zbufSegPrev - get the previous segment in a zbuf
*
* This routine finds the zbuf segment in <zbufId> that is just previous
* to the zbuf segment <zbufSeg>.  If <zbufSeg> is NULL, or is the first
* segment in <zbufId>, NULL is returned.
*
* PROTECTION DOMAINS
* This function can only be called from within the kernel protection domain.
* All arguments MUST be valid in the kernel protection domain.  Returned value
* is valid in the protection domain only.
*
* RETURNS:
* The zbuf segment ID of the segment previous to <zbufSeg>,
* or NULL if the operation fails.
*/

ZBUF_SEG
zbufSegPrev(
    ZBUF_ID         zbufId,     /* zbuf to examine */
    ZBUF_SEG        zbufSeg     /* segment to get previous segment */
    )
{
    ZBUF_SEG        zbuf;

    if ( zbufId == (ZBUF_ID)NULL )
    {
        errnoSet(S_zbufLib_ID_INVALID);
        return NULL;
    }

    zbuf = ZBUF_GETSEG(zbufId);                    /* zbuf in chain */

    if ( (zbufSeg == NULL) || (zbufSeg == zbuf) )
        return NULL;           /* no previous to first zbuf */

    /*
     * Find <zbufSeg> and return a pointer to the previous zbuf.
     * Note: OK if <zbufSeg> is not found... the return will be NULL anyway.
     */

    for ( ; (zbuf != NULL) && (ZSEG_NEXT(zbuf) != zbufSeg); zbuf = ZSEG_NEXT(zbuf) )
        ;

    if ( zbuf == NULL )             /* could not find zbufSeg */
    {
        errnoSet(S_zbufLib_SEGMENT_NOT_FOUND);
    }

    return zbuf;
}

/*******************************************************************************
*
* zbufSegData - determine the location of data in a zbuf segment
*
* This routine returns the location of the first byte of data in the zbuf
* segment <zbufSeg>.  If <zbufSeg> is NULL, the location of data in the
* first segment in <zbufId> is returned.
*
* PROTECTION DOMAINS
* This function can only be called from within the kernel protection domain.
* All arguments MUST be valid in the kernel protection domain.  Returned value
* is valid in the protection domain only.
*
* RETURNS:
* A pointer to the first byte of data in the specified zbuf segment,
* or NULL if the operation fails.
*
*/

caddr_t
zbufSegData(
    ZBUF_ID         zbufId,     /* zbuf to examine */
    ZBUF_SEG        zbufSeg     /* segment to get pointer to data */
    )
{
    ZBUF_SEG seg;
    if ( zbufId == (ZBUF_ID)NULL )
    {
        errnoSet(S_zbufLib_ID_INVALID);
        return NULL;
    }

    seg = ZBUF_GETSEG(zbufId);
    if ( seg == NULL )     /* is this chain empty ? */
    {
        errnoSet(S_zbufLib_ID_EMPTY);
        return NULL;
    }

    /* get data location <zbufSeg> (or start of chain if <zbufSeg> is NULL) */
    if ( zbufSeg == NULL )
        return ztod(seg, caddr_t);
    return ztod(zbufSeg, caddr_t);
}

/*******************************************************************************
*
* zbufSegLength - determine the length of a zbuf segment
*
* This routine returns the number of bytes in the zbuf segment <zbufSeg>.
* If <zbufSeg> is NULL, the length of the first segment in <zbufId> is
* returned.
*
* PROTECTION DOMAINS
* This function can only be called from within the kernel protection domain.
* All arguments MUST be valid in the kernel protection domain.
*
* RETURNS:
* The number of bytes in the specified zbuf segment,
* or ERROR if the operation fails.
*/

int
zbufSegLength(
    ZBUF_ID         zbufId,     /* zbuf to examine */
    ZBUF_SEG        zbufSeg     /* segment to determine length of */
    )
{
    if ( zbufId == (ZBUF_ID)NULL )
    {
        errnoSet(S_zbufLib_ID_INVALID);
        return ERROR;
    }

    if ( ZBUF_GETSEG(zbufId) == NULL )     /* is this chain empty ? */
    {
        if ( zbufSeg == NULL )      /* empty zbuf chain */
            return 0;

        errnoSet(S_zbufLib_SEGMENT_NOT_FOUND);
        return ERROR;
    }

    /* get length of <zbufSeg> (or start of chain if <zbufSeg> is NULL) */
    if ( zbufSeg == NULL )
        return ZSEG_LEN( ZBUF_GETSEG(zbufId) );
    else
        return ZSEG_LEN(zbufSeg);
}
#else
int ipcom_zbufLib_empty_file;
#endif /* IPCOM_ZEROCOPY */

