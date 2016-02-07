/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_inet6.c,v $ $Revision: 1.4 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_inet6.c,v $
 *   $Author: kenneth $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * IPv6 externsion header manipulation function defined in RFC 3542.
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

#ifdef IPCOM_USE_INET6

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_clib.h>
#include <ipcom_sock.h>
#include <ipcom_sock6.h>


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPCOM_INET6_SET_ADDR(dst, src) ipcom_memcpy(dst, src, sizeof(struct Ip_in6_addr))


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
IP_PUBLIC IP_CONST struct Ip_in6_addr ip_in6addr_any      = IP_IN6ADDR_ANY_INIT;
IP_PUBLIC IP_CONST struct Ip_in6_addr ip_in6addr_loopback = IP_IN6ADDR_LOOPBACK_INIT;

/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_inet6_rth_space
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_socklen_t
ipcom_inet6_rth_space(int type, int segments)
{
    if (type != IP_IPV6_RTHDR_TYPE_0)
        return 0;

    /* round up to nearest multiple of 8 */
    return ((sizeof(Ip_pkt_ip6_rthdr0) + segments * sizeof(struct Ip_in6_addr) + 7) / 8) * 8;
}


/*
 *===========================================================================
 *                    ipcom_inet6_rth_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void *
ipcom_inet6_rth_init(void *bp, Ip_socklen_t bp_len, int type, int segments)
{
    Ip_pkt_ip6_rthdr *rthdr = (Ip_pkt_ip6_rthdr *) bp;
    Ip_socklen_t      required_bp_len;

    required_bp_len = ipcom_inet6_rth_space(type, segments);
    if (required_bp_len == 0 /* Unsupported type or too many segments*/
        || bp_len < required_bp_len /* Too small buffer */)
        return IP_NULL;

    ipcom_memset(rthdr, 0, bp_len);
    rthdr->ip6r_type = (Ip_u8) type;
    rthdr->ip6r_nxt  = IP_IPPROTO_NONE;
    return rthdr;
}


/*
 *===========================================================================
 *                    ipcom_inet6_rth_add
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_inet6_rth_add(void *bp, const struct Ip_in6_addr *addr)
{
    Ip_pkt_ip6_rthdr0  *rthdr = (Ip_pkt_ip6_rthdr0 *) bp;
    struct Ip_in6_addr *rtaddr;

    if (rthdr->ip6r0_type != IP_IPV6_RTHDR_TYPE_0)
        return -1;

    rtaddr = (struct Ip_in6_addr *) (rthdr + 1);
    ipcom_memcpy(&rtaddr[rthdr->ip6r0_segleft], addr, sizeof(struct Ip_in6_addr));
    ++rthdr->ip6r0_segleft;
    rthdr->ip6r0_len += sizeof(struct Ip_in6_addr) / 8;

    return 0;
}


/*
 *===========================================================================
 *                    ipcom_inet6_rth_reverse
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_inet6_rth_reverse(const void *in, void *out)
{
    Ip_pkt_ip6_rthdr   *rthdr = (Ip_pkt_ip6_rthdr *) in;
    struct Ip_in6_addr  temp_addr;
    struct Ip_in6_addr *addr1;
    struct Ip_in6_addr *addr2;
    int i;
    int segments;

    segments = ipcom_inet6_rth_segments(in);
    if (segments < 0)
        return -1;

    if (in != out)
    {
        ipcom_memcpy(out, in, (rthdr->ip6r_len + 1) * 8);
        return ipcom_inet6_rth_reverse(out, out);
    }

    for (i = 0; i < segments / 2; i++)
    {
        addr1 = ipcom_inet6_rth_getaddr(out, i);
        addr2 = ipcom_inet6_rth_getaddr(out, segments - i - 1);
        IPCOM_INET6_SET_ADDR(&temp_addr, addr1);
        IPCOM_INET6_SET_ADDR(addr1, addr2);
        IPCOM_INET6_SET_ADDR(addr2, &temp_addr);
    }
    return 0;
}


/*
 *===========================================================================
 *                    ipcom_inet6_rth_segments
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_inet6_rth_segments(const void *bp)
{
    Ip_pkt_ip6_rthdr0 *rthdr = (Ip_pkt_ip6_rthdr0 *) bp;

    if (rthdr->ip6r0_type != IP_IPV6_RTHDR_TYPE_0)
        /* Unsupported type */
        return IP_NULL;

    return rthdr->ip6r0_len / 2;
}


/*
 *===========================================================================
 *                    ipcom_inet6_rth_getaddr
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC struct Ip_in6_addr *
ipcom_inet6_rth_getaddr(const void *bp, int i)
{
    Ip_pkt_ip6_rthdr0  *rthdr = (Ip_pkt_ip6_rthdr0 *) bp;
    struct Ip_in6_addr *rtaddr;

    if (rthdr->ip6r0_type != IP_IPV6_RTHDR_TYPE_0)
        /* Unsupported type */
        return IP_NULL;

    if (ipcom_inet6_rth_space(rthdr->ip6r0_type, i) > (Ip_socklen_t) (rthdr->ip6r0_len + 1) * 8)
        /* Index out of range */
        return IP_NULL;

    rtaddr = (struct Ip_in6_addr *) (rthdr + 1);
    return &rtaddr[i];
}


/*
 *===========================================================================
 *                    ipcom_inet6_opt_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_inet6_opt_init(void *extbuf, Ip_socklen_t extlen)
{
    Ip_pkt_ip6_ext_hdr *ext_hdr;

    if (extbuf != IP_NULL)
    {
        if ((extlen & 7) != 0 || extlen < sizeof(Ip_pkt_ip6_ext_hdr))
            return -1;

        ext_hdr = (Ip_pkt_ip6_ext_hdr *) extbuf;
        /* Fill the buffer with a patter to detect errors easier */
        ipcom_memset(extbuf, 0xbe, extlen);
        ext_hdr->nxt = IP_IPPROTO_NONE;
        ext_hdr->len = 0;
    }

    return sizeof(Ip_pkt_ip6_ext_hdr);
}


/*
 *===========================================================================
 *                    ipcom_inet6_opt_append
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_inet6_opt_append(void *extbuf, Ip_socklen_t extlen, int offset,
    Ip_u8 type, Ip_socklen_t len, unsigned int align, void **databufp)
{
    Ip_pkt_ip6_ext_hdr *ext_hdr;
    Ip_pkt_ip6_opt     *opt;
    Ip_socklen_t        align_offset;
    Ip_socklen_t        i;
    Ip_socklen_t        total_len;

    if ((align != 1 && align != 2 && align != 4 && align != 8)
        || align > len)
        return -1;

    align_offset = offset + ((align - offset) & (align - 1));
    total_len = align_offset + sizeof(Ip_pkt_ip6_opt) + len;

    if (extbuf != IP_NULL)
    {
        if ((extlen & 7) != 0 || extlen < total_len)
            return -1;

        for (i = offset; i < align_offset; i++)
            *((Ip_u8 *) extbuf + i) = IP_IP6OPT_PAD1;

        opt = (Ip_pkt_ip6_opt *) ((Ip_u8 *) extbuf + align_offset);
        opt->ip6o_len = (Ip_u8)len;
        opt->ip6o_type = type;
        *databufp = opt + 1;

        ext_hdr = (Ip_pkt_ip6_ext_hdr *) extbuf;
        ext_hdr->len = (Ip_u8)(total_len >> 3);
    }

    return (int) total_len;
}


/*
 *===========================================================================
 *                    ipcom_inet6_opt_finish
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_inet6_opt_finish(void *extbuf, Ip_socklen_t extlen, int offset)
{
    Ip_socklen_t total_len;
    Ip_socklen_t i;

    total_len = offset + ((8 - offset) & 7);

    if (extbuf != IP_NULL)
    {
        if ((extlen & 7) != 0 || extlen < total_len)
            return -1;

        for (i = offset; i < total_len; i++)
            *((Ip_u8 *) extbuf + i) = IP_IP6OPT_PAD1;
    }

    return (int) total_len;
}


/*
 *===========================================================================
 *                    ipcom_inet6_opt_set_val
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_inet6_opt_set_val(void *databuf, int offset, void *val, Ip_socklen_t vallen)
{
    ipcom_memcpy((Ip_u8 *) databuf + offset, val, vallen);
    return offset + (int)vallen;
}


/*
 *===========================================================================
 *                    ipcom_inet6_opt_next
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_inet6_opt_next(void *extbuf, Ip_socklen_t extlen, int offset,
                     Ip_u8 *typep, Ip_socklen_t *lenp, void **databufp)
{
    Ip_socklen_t    current_offset;
    Ip_pkt_ip6_opt *opt = IP_NULL;

    current_offset = IP_MAX((int)sizeof(Ip_pkt_ip6_ext_hdr), offset);

    while (current_offset < extlen)
    {
        /* Skip PAD1 and PADN */
        opt = (Ip_pkt_ip6_opt *) ((Ip_u8 *) extbuf + current_offset);

        if (opt->ip6o_type == IP_IP6OPT_PAD1)
        {
            current_offset++;
            continue;
        }

        if (current_offset + sizeof(Ip_pkt_ip6_opt) > extlen
            || current_offset + opt->ip6o_len > extlen)
            /* Extends beyond the buffer */
            return -1;

        if (opt->ip6o_type == IP_IP6OPT_PADN)
        {
            current_offset += opt->ip6o_len + sizeof(Ip_pkt_ip6_ext_hdr);
            continue;
        }
        break;
    }

    if (current_offset >= extlen)
        /* No more options to return */
        return -1;

    *typep = opt->ip6o_type;
    *lenp = opt->ip6o_len;
    *databufp = opt + 1;

    return (int)(current_offset + opt->ip6o_len + sizeof(Ip_pkt_ip6_ext_hdr));
}


/*
 *===========================================================================
 *                    ipcom_inet6_opt_find
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_inet6_opt_find(void *extbuf, Ip_socklen_t extlen, int offset,
                     Ip_u8 type, Ip_socklen_t *lenp, void **databufp)
{
    Ip_u8 this_type;

    do
    {
        offset = ipcom_inet6_opt_next(extbuf, extlen, offset,
            &this_type, lenp, databufp);
    } while (offset > 0 && this_type != type);

    return offset;
}


/*
 *===========================================================================
 *                    ipcom_inet6_opt_get_val
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_inet6_opt_get_val(void *databuf, int offset, void *val, Ip_socklen_t vallen)
{
    ipcom_memcpy(val, (Ip_u8 *) databuf + offset, vallen);
    return offset + (int)vallen;
}

#else

void ipcom_inet6_empty_file(void);
void ipcom_inet6_empty_file(void) {}

#endif
/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
