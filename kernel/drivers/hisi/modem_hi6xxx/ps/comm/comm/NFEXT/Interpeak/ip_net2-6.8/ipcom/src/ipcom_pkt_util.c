/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_pkt_util.c,v $ $Revision: 1.7 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_pkt_util.c,v $
 *     $Author: jhorteli $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */

/*
 ****************************************************************************
 *                    DESCRIPTION
 ****************************************************************************
 * Implements a few generic Ipcom_pkt manipulation functions.
 */


/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#ifndef IPCOM_KERNEL
#define IPCOM_KERNEL
#endif
#include "ipcom_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_os.h>
#include <ipcom_pkt.h>
#include <ipcom_sock.h>
#include <ipcom_type.h>

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

/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
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
 * 10                   STATIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                             ipcom_pkt_copy
 *===========================================================================
 * Description: Copies the content of the source packet into the destination
 *              packet. The source packet may consist of multiple parts.
 * Parameters:  dst - destination packet
 *              src - source packet
 * Returns:
 *
 */
IP_STATIC void
ipcom_pkt_copy(Ipcom_pkt *dst, Ipcom_pkt *src)
{
    if (src == IP_NULL)
        return;

    ipcom_memcpy(&dst->data[dst->end],
                 &src->data[src->start],
                 src->end - src->start);
    dst->end += src->end - src->start;

    ipcom_pkt_copy(dst, src->next_part);
}


/*
 ****************************************************************************
 * 11                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 12                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_pkt_alloc_info
 *===========================================================================
 * Description: Allocates packet specific information in the specified
 *                    packet. Caller must make sure that the allocated
 *                    info types does not already exist in the packet.
 * Parameters:  pkt - packet from which the informtion buffer will be
 *                    allocated
 *              info_type - type of information to add
 *              info_size - number of octets that should be allocated.
 * Returns:     A pointer to the space where the packet specific
 *                    information can be stored.
 *
 */
IP_PUBLIC void *
ipcom_pkt_alloc_info(Ipcom_pkt *pkt,
                     Ip_u8 info_type,
                     Ip_size_t info_size)
{
    Ip_u8 *info_type_ptr = (Ip_u8*) &pkt->extra_data[pkt->next_extra_data];
    Ip_u8 *info_len_ptr = info_type_ptr + 1;
    void  *info_data_ptr = &pkt->extra_data[pkt->next_extra_data + 1];

    *info_type_ptr = info_type;
    *info_len_ptr  = 1 + (info_size + sizeof(Ip_ptrdiff_t) - 1) / sizeof(Ip_ptrdiff_t);
    pkt->next_extra_data += *info_len_ptr;

    if (IP_UNLIKELY(pkt->next_extra_data * sizeof(Ip_ptrdiff_t)
                    > sizeof(pkt->extra_data)))
    {
        IP_PANIC();
        return IP_NULL;
    }

    return info_data_ptr;
}


/*
 *===========================================================================
 *                    ipcom_pkt_set_info
 *===========================================================================
 * Description: Sets the value of the specified info type. Any old
 *                    value already present will be overwritten.
 * Parameters:  pkt - packet to which the informtion will be set.
 *              info_type - type of information to add
 *              info_size - number of octets that should be allocated.
 *              new_info - pointer to the new info that will be copied
 *                    into the packet info buffer
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_pkt_set_info(Ipcom_pkt *pkt,
                   Ip_u8 info_type,
                   Ip_size_t info_size,
                   void *new_info)
{
    void *info;

    info = ipcom_pkt_get_info(pkt, info_type);
    if (info == IP_NULL)
        info = ipcom_pkt_alloc_info(pkt, info_type, info_size);
    ipcom_memmove(info, new_info, info_size);
}


/*
 *===========================================================================
 *                       ipcom_pkt_get_info
 *===========================================================================
 * Description: Gets a specific packet information buffer.
 * Parameters:  pkt - packet from which the informtion should be returned
 *              info_type - type of information to search for
 * Returns:     A pointer to the packet information or IP_NULL if no
 *                       information of the specified type has been stored.
 */
IP_PUBLIC void *
ipcom_pkt_get_info(IP_CONST Ipcom_pkt *pkt, Ip_u8 info_type)
{
    Ip_u8    *info_type_ptr;
    Ip_u8    *info_len_ptr;
    unsigned  i;

    for (i = 0;
         i < pkt->next_extra_data;
         i += *info_len_ptr)
    {
        info_type_ptr = (Ip_u8 *)&pkt->extra_data[i];
        info_len_ptr = info_type_ptr + 1;

        ip_assert(*info_len_ptr > 0);

        if (*info_type_ptr == info_type)
            return (void *) &pkt->extra_data[i + 1];

    }

    return IP_NULL;
}


/*
 *===========================================================================
 *                       ipcom_pkt_get_info_safe
 *===========================================================================
 * Description: Identical to ipcom_pkt_get_info() except that this
 *                       function will never return IP_NULL. A pointer
 *                       to a static buffer containing only zeros is
 *                       returned if the value is not present.
 * Parameters:  pkt - packet from which the informtion should be returned
 *              info_type - type of information to search for
 * Returns:     A pointer to the packet information.
 */
IP_PUBLIC void *
ipcom_pkt_get_info_safe(IP_CONST Ipcom_pkt *pkt, Ip_u8 info_type)
{
    static Ip_ptrdiff_t zero = 0;
    void               *info;

    info = ipcom_pkt_get_info(pkt, info_type);
    if (info)
        return info;
    return &zero;
}


/*
 *===========================================================================
 *                          ipcom_pkt_copy_info
 *===========================================================================
 * Description: Copies all packet info that has the copy flag set.
 * Parameters:  dst - Packet the information should be copied to
 *              src - Packet to copy from
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_pkt_copy_info(Ipcom_pkt *dst, IP_CONST Ipcom_pkt *src)
{
    unsigned i;
    Ip_u8   *src_info_ptr;

    for (i = 0; i < src->next_extra_data; i += src_info_ptr[1])
    {
        src_info_ptr = (Ip_u8 *)&src->extra_data[i];

        if ((src_info_ptr[0] & IPCOM_PKT_INFO_COPY) != 0)
        {
            /* Remove the Header; recalc to bytes */
            Ip_size_t isize = (src_info_ptr[1] - 1) * sizeof(Ip_ptrdiff_t);
            Ip_u8    *dst_info_ptr;

            /* Alloc and copy */
            dst_info_ptr = ipcom_pkt_alloc_info(dst, src_info_ptr[0], isize);
            ipcom_memcpy(dst_info_ptr, &src->extra_data[i + 1], isize);
        }
    }
}


/*
 *===========================================================================
 *                          ipcom_pkt_get_length
 *===========================================================================
 * Description: Gets length data area of a (possibly multi-part) packet.
 * Parameters:  pkt - a packet
 * Returns:     Number of octets the data area consists of.
 *
 */
IP_PUBLIC int
ipcom_pkt_get_length(Ipcom_pkt *pkt)
{
    int len = 0;

    do {
        len += pkt->end - pkt->start;
        pkt = pkt->next_part;
    } while (pkt != IP_NULL);

    return len;
}


/*
 *===========================================================================
 *                           ipcom_pkt_get_data
 *===========================================================================
 * Description: Returns a pointer to the data area at the specified offset.
 * Parameters:  pkt - possibly a multi-part packet.
 *              offset - number of octets into the data buffer.
 * Returns:     Pointer to the data area.
 *
 */
IP_PUBLIC void *
ipcom_pkt_get_data(Ipcom_pkt *pkt, int offset)
{
    while (offset >= pkt->end - pkt->start)
    {
        offset -= pkt->end - pkt->start;
        pkt = pkt->next_part;
    }
    return &pkt->data[pkt->start + offset];
}


/*
 *===========================================================================
 *                         ipcom_pkt_make_linear
 *===========================================================================
 * Description: Creates one Ipcom_pkt instance out of a chain of Ipcom_pkt:s
 * Parameters:  pkt - packet to make linear
 * Returns:     A linear packet.
 *
 */
IP_PUBLIC Ipcom_pkt *
ipcom_pkt_make_linear(Ipcom_pkt *pkt)
{
    Ipcom_pkt *lpkt;
    int        pkt_len;

    if (pkt->next_part == IP_NULL)
        /* Packet is already linear */
        return pkt;

    pkt_len = ipcom_pkt_get_length(pkt);
    lpkt = ipcom_pkt_malloc(pkt_len,
                            IP_FLAG_FC_STACKCONTEXT);

    if (IP_LIKELY(lpkt != IP_NULL))
    {
        ipcom_memcpy(lpkt, pkt, ip_offsetof(Ipcom_pkt, maxlen));
        ipcom_pkt_copy_info(lpkt, pkt);

        lpkt->start = (lpkt->maxlen - pkt_len) & ~(sizeof(void*) - 1);
        lpkt->end = lpkt->start;
        ipcom_pkt_copy(lpkt, pkt);
    }
    ipcom_pkt_free(pkt);

    return lpkt;
}

