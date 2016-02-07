/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_buffer.h,v $ $Revision: 1.12 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_buffer.h,v $
 *     $Author: lob $ $Date: 2004-09-20 10:52:01 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Roger Boden <roger@interpeak.se>
 ******************************************************************************
 */
#ifndef IPCOM_BUFFER_H
#define IPCOM_BUFFER_H

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


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include "ipcom_type.h"
#include "ipcom_cstyle.h"
#include "ipcom_syslog.h"

#ifdef __cplusplus
extern "C" {
#endif

  /*
****************************************************************************
* 4                    DEFINES
****************************************************************************
*/

  /*
****************************************************************************
* 5                    TYPES
****************************************************************************
*/

typedef struct Ipcom_buffer_struct
{
  Ip_u8   *buf;	     /* Buffer for data. */
  Ip_u32   alloc;    /* Number of bytes allocated for data. */
  Ip_u32   offset;   /* Offset of first byte containing data. */
  Ip_u32   end;	     /* Offset of last byte containing data. */
}
Ipcom_buffer;


/*
****************************************************************************
* 6                    FUNCTIONS
****************************************************************************
*/

IP_PUBLIC void ipcom_buffer_init(Ipcom_buffer *buffer);

/* Frees any memory used for the buffer. */
IP_PUBLIC void ipcom_buffer_free(Ipcom_buffer *buffer);

/* Clears any data from the buffer, making it empty.  This does not actually
   zero the memory. */
IP_PUBLIC void ipcom_buffer_clear(Ipcom_buffer *buffer);

/* Appends data to the buffer, expanding it if necessary. */
IP_PUBLIC Ip_err ipcom_buffer_put(Ipcom_buffer *buffer, const Ip_u8 *data, Ip_u32 len);

IP_PUBLIC Ip_err ipcom_buffer_put_char(Ipcom_buffer *buffer, const Ip_u8 data);

IP_PUBLIC Ip_err ipcom_buffer_put_int(Ipcom_buffer *buffer, const Ip_s32 data);

/*
 * Appends space to the buffer, expanding the buffer if necessary. This does
 * not actually copy the data into the buffer, but instead returns a pointer
 * to the allocated region.
 */
IP_PUBLIC Ip_err ipcom_buffer_append_space(Ipcom_buffer *buffer, Ip_u8 **datap, Ip_u32 len);

/* Returns the number of bytes of data in the buffer. */
IP_PUBLIC Ip_u32 ipcom_buffer_len(Ipcom_buffer *buffer);

/* Returns the number of free bytes in the buffer. */
IP_PUBLIC Ip_u32 ipcom_buffer_capacity(Ipcom_buffer *buffer);

/* Gets data from the beginning of the buffer. */
IP_PUBLIC Ip_err ipcom_buffer_get(Ipcom_buffer *buffer, Ip_u8 *buf, Ip_u32 len);

IP_PUBLIC Ip_err ipcom_buffer_get_int(Ipcom_buffer *buffer, Ip_s32 *data);
IP_PUBLIC Ip_err ipcom_buffer_get_uint(Ipcom_buffer *buffer, Ip_u32 *data);
IP_PUBLIC Ip_err ipcom_buffer_get_char(Ipcom_buffer *buffer, Ip_u8 *data);

/* Consumes the given number of bytes from the beginning of the buffer. */
IP_PUBLIC Ip_err ipcom_buffer_consume(Ipcom_buffer *buffer, Ip_u32 bytes);

/* Consumes the given number of bytes from the end of the buffer. */
IP_PUBLIC Ip_err ipcom_buffer_consume_end(Ipcom_buffer *buffer, Ip_u32 bytes);

/* Returns a pointer to the first used byte in the buffer. */
IP_PUBLIC Ip_u8 *ipcom_buffer_start_ptr(Ipcom_buffer *buffer);

/* Returns a pointer to the first free byte in the buffer. */
IP_PUBLIC Ip_u8 *ipcom_buffer_end_ptr(Ipcom_buffer *buffer);

IP_PUBLIC void ipcom_buffer_reset(Ipcom_buffer *buffer);

IP_PUBLIC Ipcom_buffer *ipcom_buffer_new(Ip_s32 size);

/* Decreases the amount of free bytes in the buffer. */
IP_PUBLIC Ip_err ipcom_buffer_decr_cap(Ipcom_buffer *buffer, Ip_u32 len);

/* If the buffer is empty, it is reset so any new data gets written to
 * the beginning of the buffer. If the buffer contains data, but the
 * first half of the buffer is empty, the data is move to the beginning
 * of the buffer. */
IP_PUBLIC void ipcom_buffer_trim(Ipcom_buffer *buffer);

IP_PUBLIC int ipcom_buffer_start_pos(Ipcom_buffer *buffer);
IP_PUBLIC int ipcom_buffer_end_pos(Ipcom_buffer *buffer);
IP_PUBLIC char* ipcom_buffer_ptr2pos(Ipcom_buffer *buffer, int pos);

/*
 * Dumps the contents of the buffer to stdout in hex.  This is intended for
 * debugging purposes only.
 */
IP_PUBLIC void ipcom_buffer_dump(Ipcom_buffer *buffer);
IP_PUBLIC void ipcom_buffer_dump_len(Ipcom_buffer *buffer, Ip_s32 len);

#ifdef __cplusplus
}
#endif

#endif


/*
****************************************************************************
*                      END OF FILE
****************************************************************************
*/
