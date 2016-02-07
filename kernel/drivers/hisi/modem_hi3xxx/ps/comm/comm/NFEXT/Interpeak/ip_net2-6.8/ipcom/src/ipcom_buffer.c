/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_buffer.c,v $ $Revision: 1.17 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_buffer.c,v $
 *     $Author: lob $ $Date: 2004-09-20 09:18:45 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *        Design and implementation by Roger Boden <roger@interpeak.se>
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipcom_config.h"

/* Maximum size of ipcom_buffer.h API buffers. */
#define IPCOM_BUFFER_SIZE   4096

#ifndef IPCOM_DEBUG_BUFFER
#undef IPCOM_USE_SYSLOG
#endif

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO
#include "ipcom_buffer.h"
#include "ipcom_err.h"
#include "ipcom_clib.h"


/*
 ****************************************************************************
 * 4                    EXTERN PROTOTYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 5                    DEFINES
 ****************************************************************************
 */

#define IPCOM_SYSLOG_PRIORITY    IPCOM_SYSLOG_IPCOM_PRIORITY
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPCOM


/*
 ****************************************************************************
 * 6                    TYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_buffer_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_buffer_init(Ipcom_buffer *buffer)
{
    buffer->alloc = IPCOM_BUFFER_SIZE;
    buffer->buf = ipcom_malloc(IPCOM_BUFFER_SIZE);
    buffer->offset = 0;
    buffer->end = 0;
}


/*
 *===========================================================================
 *                    ipcom_buffer_new
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
Ipcom_buffer*
ipcom_buffer_new(Ip_s32 size)
{
    Ipcom_buffer *buffer;

    buffer = ipcom_malloc(sizeof(Ipcom_buffer));
    if(!buffer)
    {
        IPCOM_LOG0(ERR, "ipcom_buffer_new() :: out of memory");
        return IP_NULL;
    }

    buffer->buf = ipcom_malloc(size);
    if(buffer->buf == IP_NULL)
    {
        IPCOM_LOG0(ERR, "ipcom_buffer_new() :: out of memory");
        ipcom_free(buffer);
        return IP_NULL;
    }

    buffer->alloc = size;
    buffer->offset = 0;
    buffer->end = 0;

#ifdef IP_DEBUG
    ipcom_memset(buffer->buf, 0xcc, size);
#endif

    return buffer;
}


/*
 *===========================================================================
 *                    ipcom_buffer_free
 *===========================================================================
 * Description: Frees any memory used for the buffer.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_buffer_free(Ipcom_buffer *buffer)
{
#ifdef IP_DEBUG
    ipcom_memset(buffer->buf, 0xdd, buffer->alloc);
#endif
    ip_assert( buffer->buf && buffer );
    ipcom_free(buffer->buf);
    ipcom_free(buffer);
}


/*
 *===========================================================================
 *                    ipcom_buffer_reset
 *===========================================================================
 * Description: Clears any data from the buffer, making it empty and ready
 *              to reuse.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_buffer_reset(Ipcom_buffer *buffer)
{
    buffer->offset = 0;
    buffer->end = 0;
}


/*
 *===========================================================================
 *                    ipcom_buffer_put
 *===========================================================================
 * Description: Appends data to the buffer, expanding it if necessary.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_buffer_put(Ipcom_buffer *buffer, const Ip_u8 *data, Ip_u32 len)
{
    Ip_u8 *cp;
    if(ipcom_buffer_append_space(buffer, &cp, len))
        return IPCOM_ERR_FAILED;

    ipcom_memcpy(cp, data, len);

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_buffer_put_char
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_buffer_put_char(Ipcom_buffer *buffer, const Ip_u8 data)
{
    return ipcom_buffer_put(buffer, (Ip_u8 *)&data, 1);
}


/*
 *===========================================================================
 *                    ipcom_buffer_put_int
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_buffer_put_int(Ipcom_buffer *buffer, const Ip_s32 data)
{
    return ipcom_buffer_put(buffer, (Ip_u8 *)&data, 4);
}


/*
 *===========================================================================
 *                    ipcom_buffer_get_int
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_buffer_get_int(Ipcom_buffer *buffer, Ip_s32 *data)
{
    return ipcom_buffer_get(buffer, (Ip_u8 *)data, 4);
}


/*
 *===========================================================================
 *                    ipcom_buffer_get_uint
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_buffer_get_uint(Ipcom_buffer *buffer, Ip_u32 *data)
{
    return ipcom_buffer_get(buffer, (Ip_u8 *)data, 4);
}


/*
 *===========================================================================
 *                    ipcom_buffer_get_char
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_buffer_get_char(Ipcom_buffer *buffer, Ip_u8 *data)
{
    return ipcom_buffer_get(buffer, (Ip_u8 *)data, 1);
}


/*
 *===========================================================================
 *                    ipcom_buffer_append_space
 *===========================================================================
 * Description: Appends space to the buffer, expanding the buffer if necessary.
 *              This does not actually copy the data into the buffer, but
 *              instead returns a pointer to the allocated region.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_buffer_append_space(Ipcom_buffer *buffer, Ip_u8 **datap, Ip_u32 len)
{
    /* If the buffer is empty, start using it from the beginning. */
    if (buffer->offset == buffer->end)
    {
        buffer->offset = 0;
        buffer->end = 0;
    }
restart:
    /* If there is enough space to store all data, store it now. */
    if (buffer->end + len <= buffer->alloc)
    {
        *datap = buffer->buf + buffer->end;
        buffer->end += len;
        return IPCOM_SUCCESS;
    }
    /*
    * If the buffer is quite empty, but all data is at the end, move the
    * data to the beginning and retry.
    */
    if (buffer->offset > buffer->alloc / 2)
    {
        ipcom_memmove(buffer->buf, buffer->buf + buffer->offset,
            buffer->end - buffer->offset);
        buffer->end -= buffer->offset;
        buffer->offset = 0;
        goto restart;
    }
    /* Increase the size of the buffer and retry. */
    buffer->alloc += buffer->alloc*2;
    buffer->buf = ipcom_realloc(buffer->buf, buffer->alloc);
    if( !buffer->buf )
        return IPCOM_ERR_FAILED;
    goto restart;
}


/*
 *===========================================================================
 *                    ipcom_buffer_len
 *===========================================================================
 * Description: Returns the number of bytes of data in the buffer.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_u32
ipcom_buffer_len(Ipcom_buffer *buffer)
{
    IPCOM_LOG4(DEBUG2,
               "ipcom_buffer_len :: buf=%x, len=%d, end=%d, offset=%d",
               buffer, buffer->end - buffer->offset, buffer->end, buffer->offset);

    return buffer->end - buffer->offset;
}


/*
 *===========================================================================
 *                    ipcom_buffer_capacity
 *===========================================================================
 * Description: Returns the number of free bytes in the buffer.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_u32
ipcom_buffer_capacity(Ipcom_buffer * buffer)
{
    return buffer->alloc - buffer->end;
}


/*
 *===========================================================================
 *                    ipcom_buffer_get
 *===========================================================================
 * Description: Gets data from the beginning of the buffer.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_buffer_get(Ipcom_buffer *buffer, Ip_u8 *buf, Ip_u32 len)
{
    ip_assert(len <= (buffer->end - buffer->offset));

    if(len > (buffer->end - buffer->offset))
        return IPCOM_ERR_FAILED;

    ipcom_memcpy(buf, buffer->buf + buffer->offset, len);
    buffer->offset += len;
    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_buffer_consume
 *===========================================================================
 * Description: Consumes the given number of bytes from the beginning of the
 *              buffer.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_buffer_consume(Ipcom_buffer *buffer, Ip_u32 bytes)
{
    ip_assert( bytes <= (buffer->end - buffer->offset));
    if(bytes > (buffer->end - buffer->offset))
        return IPCOM_ERR_FAILED;

    IPCOM_LOG4(DEBUG2,
               "ipcom_buffer_consume :: buf=%x, bytes=%d, end=%d, offset=%d",
               buffer, bytes, buffer->end, buffer->offset);

    buffer->offset += bytes;
    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_buffer_consume_end
 *===========================================================================
 * Description: Consumes the given number of bytes from the end of the buffer.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_buffer_consume_end(Ipcom_buffer *buffer, Ip_u32 bytes)
{
    ip_assert( bytes <= (buffer->end - buffer->offset) );
    if(bytes > (buffer->end - buffer->offset))
        return IPCOM_ERR_FAILED;

    IPCOM_LOG4(DEBUG2,
               "ipcom_buffer_consume_end :: buf=%x, bytes=%d, end=%d, offset=%d",
               buffer, bytes, buffer->end, buffer->offset);

    buffer->end -= bytes;
    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_buffer_start_ptr
 *===========================================================================
 * Description: Returns a pointer to the first used byte in the buffer.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_u8 *
ipcom_buffer_start_ptr(Ipcom_buffer *buffer)
{
    IPCOM_LOG3(DEBUG2,
               "ipcom_buffer_ptr :: buf=%x, end=%d, offset=%d",
               buffer, buffer->end, buffer->offset);

    return buffer->buf + buffer->offset;
}


/*
 *===========================================================================
 *                    ipcom_buffer_end_ptr
 *===========================================================================
 * Description: Returns a pointer to the first free byte in the buffer.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_u8 *
ipcom_buffer_end_ptr(Ipcom_buffer *buffer)
{
    IPCOM_LOG3(DEBUG2,
               "ipcom_buffer_write_ptr :: buf=%x, end=%d, offset=%d",
               buffer, buffer->end, buffer->offset);

    return &(buffer->buf[buffer->end]);
}


/*
 *===========================================================================
 *                    ipcom_buffer_decr_cap
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_buffer_decr_cap(Ipcom_buffer *buffer, Ip_u32 len)
{
    ip_assert( len <= ipcom_buffer_capacity(buffer) );
    if(len > ipcom_buffer_capacity(buffer))
        return IPCOM_ERR_FAILED;

    IPCOM_LOG4(DEBUG2,
               "ipcom_buffer_decr_cap :: buf=%x, len=%d, end=%d, offset=%d",
               buffer, len, buffer->end, buffer->offset);

    buffer->end += len;
    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_buffer_trim
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_buffer_trim(Ipcom_buffer *buffer)
{
    IPCOM_LOG3(DEBUG2,
               "ipcom_buffer_trim :: buf=%x, offset=%d, end=%d",
               buffer, buffer->offset, buffer->end);

    if(buffer->end == buffer->offset)
    {
        buffer->end = 0;
        buffer->offset = 0;
        return;
    }

    /* If more than the first half of the buffer is free/unused, move the
    * data to the beginning of the buffer */
    if(buffer->offset > buffer->alloc/2)
    {
        ipcom_memmove(buffer->buf, buffer->buf + buffer->offset, buffer->end - buffer->offset);
        buffer->end -= buffer->offset;
        buffer->offset = 0;
    }
}


/*
 *===========================================================================
 *                    ipcom_buffer_start_pos
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_buffer_start_pos(Ipcom_buffer *buffer)
{
    return buffer->offset;
}


/*
 *===========================================================================
 *                    ipcom_buffer_end_pos
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_buffer_end_pos(Ipcom_buffer *buffer)
{
    return buffer->end;
}


/*
 *===========================================================================
 *                    ipcom_buffer_ptr2pos
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC char *
ipcom_buffer_ptr2pos(Ipcom_buffer *buffer, int pos)
{
    return (char *)&(buffer->buf[buffer->offset + pos]);
}


/*
*===========================================================================
*                    ipcom_buffer_dump
*===========================================================================
* Description: Dumps the contents of the buffer to stdout.
* Parameters:
* Returns:
*
*/
IP_PUBLIC void
ipcom_buffer_dump(Ipcom_buffer *buffer)
{
    ipcom_buffer_dump_len(buffer, ipcom_buffer_len(buffer));
}


/*
*===========================================================================
*                    ipcom_buffer_dump_len
*===========================================================================
* Description: Dumps the contents of the buffer to stdout.
* Parameters:
* Returns:
*
*/
IP_PUBLIC void
ipcom_buffer_dump_len(Ipcom_buffer *buffer, Ip_s32 len)
{
    int i;
    Ip_u8 *ucp = (Ip_u8 *) buffer->buf;

    for (i = buffer->offset; i < len; i++)
    {
        ipcom_debug_printf("%02x", ucp[i]);
        if ((i-buffer->offset)%16==15)
            ipcom_debug_printf("\r\n");
        else if ((i-buffer->offset)%2==1)
            ipcom_debug_printf(" ");
    }
    ipcom_debug_printf("\r\n");
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
