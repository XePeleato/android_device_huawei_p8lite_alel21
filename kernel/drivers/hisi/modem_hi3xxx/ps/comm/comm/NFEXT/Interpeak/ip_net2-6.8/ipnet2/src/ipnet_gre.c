/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_gre.c,v $ $Revision: 1.23 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_gre.c,v $
 *   $Author: jhorteli $
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
 * GRE Tunneling Interface based on the following RFC:
 * ==========================================================
 * RFC 2784 - Generic Routing Encapsulation (GRE).
 * RFC 2890 - Key and Sequence Number Extensions to GRE
 *
 * Example:
 *
 * IPNET (10.1.2.100/16):
 * > tunnel add gre0 -mode gre -local 10.1.2.100 -remote 10.1.2.64 -ttl 64 -ocsum
 * > ifconfig gre0 inet add 14.0.0.100
 * > ifconfig gre0 up
 * > route add -net -netmask 255.0.0.0 14.0.0.0 gre0
 *
 * LINUX (10.1.2.64/16):
 * > ip tunnel add gre1 mode gre remote 10.1.2.100 local 10.1.2.64 ttl 255 ocsum
 * > ip addr add 14.0.0.64 dev gre1
 * > ip link set gre1 up
 * > ip route add 14.0.0.0/8 dev gre3
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#ifndef IPCOM_KERNEL
#define IPCOM_KERNEL
#endif
#include "ipnet_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#ifdef IPNET_USE_GRE

#ifndef IPCOM_USE_INET
#error "IPCOM_USE_INET must be defined to make GRE work"
#endif

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_syslog.h>
#include <ipcom_pqueue.h>
#include <ipcom_type.h>

#include "ipnet_h.h"
#include "ipnet_netif.h"
#include "ipnet_timeout.h"
#include "ipnet_tunnel.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* Supported GRE protocol types, in network byte order */
#define IPNET_GRE_PROTOCOL_TYPE_IPV4 ip_htons(0x0800)
#define IPNET_GRE_PROTOCOL_TYPE_IPV6 ip_htons(0x86DD)


/* RFC 2784 flags: (rfc1701 options all depreceated except checksum). */
#define IPNET_GRE_FLAG_CHECKSUM   ip_htons(0x8000)  /* Checksum Present */
#define IPNET_GRE_FLAG_KEY        ip_htons(0x2000)  /* Key Present */
#define IPNET_GRE_FLAG_SEQNUM     ip_htons(0x1000)  /* Sequence Number Present */
#define IPNET_GRE_FLAG_RESERVED   ip_htons(0x03f8)  /* bits 6-12 = Reserved */
#define IPNET_GRE_FLAG_VERSION    ip_htons(0x0007)  /* bits 13-15 = Version Number */
#ifdef IPNET_USE_RFC2890
#define IPNET_GRE_FLAG_MUSTBE0    ip_htons(0x4c00)  /* bits 1,4-5 must be 0 */
#else
#define IPNET_GRE_FLAG_MUSTBE0    ip_htons(0x7c00)  /* bits 1-5 must be 0 */
#endif

#define IPNET_GRE_CHECKSUM_PRESENT(gre) \
    IP_BIT_ISSET((gre)->flags_reserved0_ver, IPNET_GRE_FLAG_CHECKSUM)
#define IPNET_GRE_KEY_PRESENT(gre) \
    IP_BIT_ISSET((gre)->flags_reserved0_ver, IPNET_GRE_FLAG_KEY)
#define IPNET_GRE_SEQNUM_PRESENT(gre) \
    IP_BIT_ISSET((gre)->flags_reserved0_ver, IPNET_GRE_FLAG_SEQNUM)

#define IPNET_GRE_HDR_SIZE         4
#define IPNET_GRE_OPT_CSUM_SIZE    4
#define IPNET_GRE_OPT_KEY_SIZE     4
#define IPNET_GRE_OPT_SEQNUM_SIZE  4


/* */
#define IPNET_GRE_PKT_SET_KEY(pkt, key) ((pkt)->link_cookie = key)

#define IPNET_GRE_PKT_GET_SEQNUM(pkt, seqnum) (Ip_u32) ((pkt)->link_cookie)
#define IPNET_GRE_PKT_SET_SEQNUM(pkt, seqnum) ((pkt)->link_cookie = seqnum)

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         Ipnet_pkt_gre
 *===========================================================================
 */
#include "ipcom_align16.h"
typedef IP_PACKED1 struct Ipnet_pkt_gre_struct
{
    Ip_u16  flags_reserved0_ver;
    Ip_u16  protocol_type;
    Ip_u8   options[1];
}
IP_PACKED2 Ipnet_pkt_gre;

typedef IP_PACKED1 struct Ipnet_pkt_gre_opt_csum_struct
{
    Ip_u16  checksum;
    Ip_u16  reserved1;
}
IP_PACKED2 Ipnet_pkt_gre_opt_csum;
#include "ipcom_align16.h"

#ifdef IPNET_USE_RFC2890
typedef struct Ipnet_gre_struct
{
    Ipcom_pqueue  *reassembly_queue;
    Ipnet_timeout *reassembly_tmo;
    Ip_u32         recv_seqnum;
    Ip_u32         send_seqnum;
}
Ipnet_gre_t;
#endif /* IPNET_USE_RFC2890 */


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
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

#ifdef IPNET_USE_RFC2890
/*
 *===========================================================================
 *                    ipnet_gre_seq_cmp
 *===========================================================================
 * Description: Compares the sequence number of two packets.
 * Parameters:  netif - A GRE tunnel interface.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_gre_seq_cmp(IP_CONST Ipcom_pkt *pkt1, IP_CONST Ipcom_pkt *pkt2)
{
    Ip_u32 seqnum1 = IP_GET_NTOHL(&pkt1->data[pkt1->start]);
    Ip_u32 seqnum2 = IP_GET_NTOHL(&pkt2->data[pkt2->start]);

    if (IPCOM_IS_LT(seqnum1, seqnum2))
        return 1;

    if (IPCOM_IS_GT(seqnum1, seqnum2))
        return -1;

    return 0;
}


/*
 *===========================================================================
 *                    ipnet_gre_tmo
 *===========================================================================
 * Description: Reassembly timeout handler.
 * Parameters:  netif - A GRE tunnel interface.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_STATIC void
ipnet_gre_seq_tmo(Ipnet_netif *netif)
{
    Ipnet_pkt_gre *gre_hdr;
    Ipcom_pkt     *pkt;
    Ipnet_gre_t   *gre = netif->ipcom.pdrv;

    /* RFC 2890, chapter 2.2
       ...
       Under no circumstances should a packet wait more that
       OUTOFORDER_TIMER milliseconds in the buffer.  If a packet has been
       waiting that long, the receiver MUST immediately traverse the buffer
       in sorted order, decapsulating packets (and ignoring any sequence
       number gaps) until there are no more packets in the buffer that have
       been waiting longer than OUTOFORDER_TIMER milliseconds. The "last
       successfully decapsulated sequence number" should then be set to the
       last packet so decapsulated.
       ...
    */
    pkt = ipcom_pqueue_get_next(gre->reassembly_queue);
    ip_assert(pkt != IP_NULL);
    gre->recv_seqnum = IP_GET_NTOHL(&pkt->data[pkt->start]);

    do
    {
        pkt = ipcom_pqueue_remove_next(gre->reassembly_queue);
        /* Discard the sequence number option and make place for a GRE header */
        pkt->start += IPNET_GRE_OPT_SEQNUM_SIZE - IPNET_GRE_HDR_SIZE;

        gre_hdr = (Ipnet_pkt_gre *)&pkt->data[pkt->start];
        gre_hdr->flags_reserved0_ver = 0;
        if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_IPV4))
            gre_hdr->protocol_type = ip_htons(0x0800);
        else if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_IPV6))
            gre_hdr->protocol_type = ip_htons(0x86DD);

        (void) ipnet_gre_input(netif, pkt);
        ++gre->recv_seqnum;
    } while (IP_NULL != (pkt = ipcom_pqueue_get_next(gre->reassembly_queue))
             && gre->recv_seqnum == IP_GET_NTOHL(&pkt->data[pkt->start]));

    if (pkt != IP_NULL)
    {
        Ip_u32 timeout = IPNET_PKT_GET_TIMEOUT_ABS(pkt);

        /* More out of order packets waiting, schedule a timeout of
           the time remaining until this packet sequence timer timeouts */
        (void) ipnet_timeout_schedule((timeout < ipnet->msec_now
                                       ? 0
                                       : timeout - ipnet->msec_now),
                                      (Ipnet_timeout_handler) ipnet_gre_seq_tmo,
                                      netif,
                                      &gre->reassembly_tmo);
    }
}
#endif /* IPNET_USE_RFC2890 */

/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_gre_setup
 *===========================================================================
 * Description: Setup the GRE tunnel.
 * Parameters:  netif - A GRE tunnel interface.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_gre_setup(Ipnet_netif *netif)
{
#ifdef IPNET_USE_RFC2890
    Ipnet_gre_t *gre;

    gre = netif->ipcom.pdrv = ipcom_calloc(1, sizeof(Ipnet_gre_t));
    if (gre == IP_NULL)
        goto errout;

    gre->reassembly_queue
        = ipcom_pqueue_new((Ipcom_pqueue_cmp_func) ipnet_gre_seq_cmp,
                           ipcom_pqueue_nop_store_index);

    return 0;
 errout:
    ipnet_gre_destroy(netif);
    IPCOM_LOG0(CRIT, "Failed to create GRE tunnel due to unsifficient memory");
    return -IP_ERRNO_ENOMEM;
#else
    IPCOM_UNUSED_ARG(netif);
    return 0;
#endif /* IPNET_USE_RFC2890 */
}


/*
 *===========================================================================
 *                    ipnet_gre_destroy
 *===========================================================================
 * Description: Destroy the GRE tunnel.
 * Parameters:  netif - A GRE tunnel interface.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_gre_destroy(Ipnet_netif *netif)
{
#ifdef IPNET_USE_RFC2890
    Ipnet_gre_t *gre = netif->ipcom.pdrv;
    Ipcom_pkt   *pkt;

    if (gre != IP_NULL)
    {
        if (gre->reassembly_queue != IP_NULL)
        {
            while (IP_NULL != (pkt = ipcom_pqueue_remove_next(gre->reassembly_queue)))
                ipcom_pkt_free(pkt);
            ipcom_pqueue_delete(gre->reassembly_queue);
        }
        ipnet_timeout_cancel(gre->reassembly_tmo);
        ipcom_free(gre);
    }
#endif /* IPNET_USE_RFC2890 */
}


/*
 *===========================================================================
 *                    ipnet_gre_input
 *===========================================================================
 * Description: Handler for received GRE packets.
 * Parameters:  pkt - Received GRE packet.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_gre_input(Ipnet_netif *netif, Ipcom_pkt *pkt)
{
    Ipnet_pkt_gre *gre_hdr;
    int            gre_hdr_start = pkt->start;

    IP_BIT_CLR(pkt->flags, IPCOM_PKT_FLAG_IPV4 | IPCOM_PKT_FLAG_IPV6);
    gre_hdr = (Ipnet_pkt_gre *)&pkt->data[pkt->start];
    pkt->start += IPNET_GRE_HDR_SIZE;

    /* First 16-bits non-null, verify bits and version. */
    if (gre_hdr->flags_reserved0_ver)
    {
        /* RFC 2784: 2.3. Reserved0 (bits 1-12)
           A receiver MUST discard a packet where any of bits 1-5 are non-zero,
           unless that receiver implements RFC 1701. Bits 6-12 are reserved for
           future use. These bits MUST be sent as zero and MUST be ignored on
           receipt.

           RFC 2890: 2. Extensions to GRE Header
           Key present (bit 2)
           Sequence Number Present (bit 3)
        */
        if (IP_BIT_ISSET(gre_hdr->flags_reserved0_ver, IPNET_GRE_FLAG_MUSTBE0))
            goto cleanup;

        /* RFC 2784: 2.3.1. Version Number (bits 13-15)
           The Version Number field MUST contain the value zero.
        */
        if (IP_BIT_ISSET(gre_hdr->flags_reserved0_ver, IPNET_GRE_FLAG_VERSION))
            goto cleanup;

        /* RFC 2784: 2.5. Checksum (2 octets)
           The Checksum field contains the IP (one's complement) checksum sum of
           the all the 16 bit words in the GRE header and the payload packet.
           For purposes of computing the checksum, the value of the checksum
           field is zero. This field is present only if the Checksum Present bit
           is set to one.
        */
        if (IPNET_GRE_CHECKSUM_PRESENT(gre_hdr))
        {
            pkt->start += IPNET_GRE_OPT_CSUM_SIZE;

            /* IP checksum must be ok. */
            if (ipcom_in_checksum(&gre_hdr->flags_reserved0_ver, pkt->end - gre_hdr_start) != 0)
                goto cleanup;
        }

#ifdef IPNET_USE_RFC2890
        /* RFC 2890: 2.1. Key Field (4 octets)
           The Key field contains a four octet number which was inserted by the
           encapsulator. The actual method by which this Key is obtained is
           beyond the scope of the document. The Key field is intended to be
           used for identifying an individual traffic flow within a tunnel. For
           example, packets may need to be routed based on context information
           not present in the encapsulated data.  The Key field provides this
           context and defines a logical traffic flow between encapsulator and
           decapsulator.  Packets belonging to a traffic flow are encapsulated
           using the same Key value and the decapsulating tunnel endpoint
           identifies packets belonging to a traffic flow based on the Key Field
           value.
        */
        if (IPNET_GRE_KEY_PRESENT(gre_hdr))
        {
            Ip_u32 key;

            key = IP_GET_NTOHL(&pkt->data[pkt->start]);
            IPNET_GRE_PKT_SET_KEY(pkt, key);
            pkt->start += IPNET_GRE_OPT_KEY_SIZE;
        }

        /* RFC 2890: 2.2. Sequence Number (4 octets)
           The Sequence Number field is a four byte field and is inserted by the
           encapsulator when Sequence Number Present Bit is set. The Sequence
           Number MUST be used by the receiver to establish the order in which
           packets have been transmitted from the encapsulator to the receiver.
           The intended use of the Sequence Field is to provide unreliable but
           in-order delivery. If the Key present bit (bit 2) is set, the
           sequence number is specific to the traffic flow identified by the Key
           field. Note that packets without the sequence bit set can be
           interleaved with packets with the sequence bit set.
        */
        if (IPNET_GRE_SEQNUM_PRESENT(gre_hdr))
        {
            Ipnet_gre_t *gre = netif->ipcom.pdrv;
            Ip_u32       seqnum;

            seqnum = IP_GET_NTOHL(&pkt->data[pkt->start]);
            if (seqnum == gre->recv_seqnum)
                /* In order segment */
                ++gre->recv_seqnum;
            else if (IPCOM_IS_LT(seqnum, gre->recv_seqnum))
                /* Out of order */
                goto cleanup;
            else
            {
                Ip_u32 outoforder_timer;
                int max_perflow_buffer;

                max_perflow_buffer = ipcom_sysvar_get_as_int0("ipnet.gre.MAX_PERFLOW_BUFFER", 3);
                outoforder_timer = ipcom_sysvar_get_as_int0("ipnet.gre.OUTOFORDER_TIMER", 1000);

                if (max_perflow_buffer <= 0)
                    /* No buffering allowed */
                    goto cleanup;

                while (ipcom_pqueue_size(gre->reassembly_queue) >= max_perflow_buffer)
                {
                    ipnet_timeout_cancel(gre->reassembly_tmo);
                    /* Call the timeout handler to force the oldest packet(s)
                       to be inputted to the stack and the receive sequence
                       number to advanced */
                    ipnet_gre_seq_tmo(netif);
                }

                if (gre_hdr->protocol_type == ip_htons(0x0800))
                    IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_IPV4);
                else if (gre_hdr->protocol_type == ip_htons(0x86DD))
                    IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_IPV6);

                IPNET_PKT_SET_TIMEOUT_ABS(pkt, ipnet->msec_now + outoforder_timer);
                if (gre->reassembly_tmo == IP_NULL)
                    if (ipnet_timeout_schedule(outoforder_timer,
                                               (Ipnet_timeout_handler) ipnet_gre_seq_tmo,
                                               netif,
                                               &gre->reassembly_tmo) < 0)
                        goto cleanup;
                if (ipcom_pqueue_insert(gre->reassembly_queue, pkt) != IPCOM_SUCCESS)
                {
                    ipnet_timeout_cancel(gre->reassembly_tmo);
                    goto cleanup;
                }
                return 0;
            }
            pkt->start += IPNET_GRE_OPT_SEQNUM_SIZE;
        }
#endif /* IPNET_USE_RFC2890 */
    }

    /**/
    if (netif->ipcom.link_tap)
        /* start is PAYLOAD == the inner IP header; ipstart points to the PREVIOUS
         * IP header */
        netif->ipcom.link_tap(&netif->ipcom, pkt, IP_PACKET_HOST, gre_hdr->protocol_type, pkt->start, pkt->start);

#ifdef IPCOM_USE_INET
    /* IPv4 in GRE. */
    if (gre_hdr->protocol_type == ip_htons(0x0800))
        return ipnet_ip4_input(netif, pkt);
#endif

#ifdef IPCOM_USE_INET6
    /* IPv6 in GRE. */
    if (gre_hdr->protocol_type == ip_htons(0x86DD))
        return ipnet_ip6_input(netif, pkt);
#endif

    /* Unsupported Protocol Type, drop. */

 cleanup:
    ipcom_pkt_free(pkt);
    return -IP_ERRNO_EINVAL;
}


/*
 *===========================================================================
 *                    ipnet_gre_output_append
 *===========================================================================
 * Description: Handler for packets to send.
 * Parameters:  pkt - Packet to send and append GRE header to.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_gre_output_append(Ipnet_netif *netif, struct Ip_ip_tunnel_param *tunnel_param, Ipcom_pkt *pkt)
{
    Ipnet_pkt_gre          *gre_hdr;
    Ipnet_pkt_gre_opt_csum *csum_opt = IP_NULL;
#ifdef IPNET_USE_RFC2890
    Ip_u32                 *key_opt = IP_NULL;
    Ip_u32                 *key_opt_info;
    Ip_u32                 *seqnum_opt = IP_NULL;
    Ipnet_gre_t            *gre = netif->ipcom.pdrv;
#endif

    ip_assert(pkt->start >= (IPNET_GRE_MAX_HDR_SIZE + IPNET_IP_HDR_SIZE));

#ifdef IPNET_USE_RFC2890
    if (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_LINK1))
    {
        pkt->start -= IPNET_GRE_OPT_SEQNUM_SIZE;
        seqnum_opt = (Ip_u32 *) &pkt->data[pkt->start];
        IP_SET_HTONL(seqnum_opt, gre->send_seqnum);
        ++gre->send_seqnum;
    }

    if ((key_opt_info = ipcom_pkt_get_info(pkt, IPNET_PKT_INFO_GRE_KEY_OUTPUT)) != IP_NULL)
    {
        pkt->start -= IPNET_GRE_OPT_KEY_SIZE;
        key_opt = (Ip_u32 *) &pkt->data[pkt->start];
        IP_SET_HTONL(key_opt, *key_opt_info);
    }
#endif /* IPNET_USE_RFC2890 */

    if (IP_BIT_ISTRUE(tunnel_param->o_flags, IPCOM_TUNNEL_FLAG_GRE_CHECKSUM)
        || IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_LINK2))
    {
        /* GRE output checksum enabled. */
        pkt->start -= IPNET_GRE_OPT_CSUM_SIZE;
        csum_opt = (Ipnet_pkt_gre_opt_csum *) &pkt->data[pkt->start];
        csum_opt->checksum  = 0;
        csum_opt->reserved1 = 0;
    }

    /* Add GRE header. */
    pkt->start -= IPNET_GRE_HDR_SIZE;
    gre_hdr = (Ipnet_pkt_gre *)&pkt->data[pkt->start];
    gre_hdr->flags_reserved0_ver = 0;
    switch (pkt->data[pkt->ipstart] & 0xf0)
    {
#ifdef IPCOM_USE_INET
    case 0x40:
        gre_hdr->protocol_type = IPNET_GRE_PROTOCOL_TYPE_IPV4;
        break;
#endif
#ifdef IPCOM_USE_INET6
    case 0x60:
        gre_hdr->protocol_type = IPNET_GRE_PROTOCOL_TYPE_IPV6;
        break;
#endif
    default:
        return -IP_ERRNO_EPROTONOSUPPORT;
    }

#ifdef IPNET_USE_RFC2890
    if (key_opt != IP_NULL)
        IP_BIT_SET(gre_hdr->flags_reserved0_ver, IPNET_GRE_FLAG_KEY);

    if (seqnum_opt != IP_NULL)
        IP_BIT_SET(gre_hdr->flags_reserved0_ver, IPNET_GRE_FLAG_SEQNUM);
#endif /* IPNET_USE_RFC2890 */

    if (csum_opt != IP_NULL)
    {
        /* The whole GRE header is written, time to calculate the checksum */
        IP_BIT_SET(gre_hdr->flags_reserved0_ver, IPNET_GRE_FLAG_CHECKSUM);
        csum_opt->checksum = ipcom_in_checksum(gre_hdr, pkt->end - pkt->start);
    }

    return 0;
}


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

#endif /* IPNET_USE_GRE */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
