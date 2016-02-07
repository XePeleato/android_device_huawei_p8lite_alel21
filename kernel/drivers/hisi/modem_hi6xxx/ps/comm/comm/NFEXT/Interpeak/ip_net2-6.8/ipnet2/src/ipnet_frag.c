/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_frag.c,v $ $Revision: 1.63.18.1.4.4 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_frag.c,v $
 *     $Author: dbush $
 *     $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *      Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Fragmentation and reassembly functions.
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

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_os.h>

#include "ipnet_h.h"

#ifdef IPCOM_WV_INSTRUMENTATION
#include <ipcom_windview.h>
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
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                          ipnet_frag_list_len
 *===========================================================================
 * Description: Returns the number of partially reassembled IP-datagrams
 *              currently present in the list.
 * Parameters:  list_head - head of the reassembly list.
 * Returns:     Number of different partially reassembled IP-datagrams
 *              in the list.
 *
 */
IP_STATIC unsigned
ipnet_frag_list_len(Ipcom_pkt *list_head)
{
    if (list_head == IP_NULL)
        return 0;
    return 1 + ipnet_frag_list_len(list_head->next);
}


/*
 *===========================================================================
 *                       ipnet_frag_dgram_list_len
 *===========================================================================
 * Description: Returns the number of partially reassembled IP-datagrams
 *              currently present in the list.
 * Parameters:  frag - a fragment
 * Returns:     Number of fragments in the IP-datagram.
 *
 */
IP_STATIC unsigned
ipnet_frag_dgram_list_len(Ipcom_pkt *frag)
{
    if (frag == IP_NULL)
        return 0;
    return 1 + ipnet_frag_dgram_list_len(frag->next_fragment);
}


/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_fragment_packet
 *===========================================================================
 * Description: Splits a packet that is to big to fit into the MTU of the
 *              network interface.
 * Parameters:  pkt - The packet to split. pkt->start is the offset to the
 *              extension header (or transport layer protocol) with the
 *              id of 'next_hdr'. This packet is released by this function.
 *              ip_hdr_size - The size of the IP header.
 *              other_hdr_size - The size of other headers that will be
 *              added due to fragmentation.
 *              mtu - The MTU of the network interface.
 *              has_more_fragments - function that returns IP_TRUE if the
 *              packet passed as an argument has more fragments. This
 *              parameter might be IP_NULL.
 * Returns:     A list of fragments or IP_NULL if not enough memory could
 *              be allocated to create all fragments.
 */
IP_GLOBAL Ipcom_pkt *
ipnet_fragment_packet(Ipcom_pkt *pkt,
                      int ip_hdr_size,
                      int other_hdr_size,
                      int mtu,
                      Ipnet_more_fragments_func more_fragments)
{
    Ipcom_pkt *frag_pkt_head;
    Ipcom_pkt *frag_pkt_tail;
    Ipcom_pkt *frag_pkt;
    int        frag_size;
    int        total_size;
    int        size_of_this_fragment;
    int        frag_offset;
    Ipnet_pkt_info_sock_snd_buf *ssb;

    total_size = pkt->end - pkt->start;
    frag_size = (mtu - ip_hdr_size - other_hdr_size) & 0xfff8;
    frag_pkt_head = IP_NULL;
    frag_pkt_tail = IP_NULL;
    frag_offset = 0;

    while (frag_offset < total_size)
    {
        if (total_size - frag_offset > frag_size)
            size_of_this_fragment = frag_size;
        else
            size_of_this_fragment = total_size - frag_offset;

        frag_pkt = ipcom_pkt_malloc(mtu, IP_FLAG_FC_STACKCONTEXT);
        if (frag_pkt == IP_NULL)
        {
            IPCOM_WV_MARKER_1 ( IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_CRITICAL, 1, 1, IPCOM_WV_NETD_NOBUFS,
                                ipnet_fragment_packet, IPCOM_WV_IPNET_FRAG_MODULE, IPCOM_WV_NETD_IP_REASSEMBLE);
            IPNET_STATS(pkt_frag_nomem++);
            ipcom_pkt_free(pkt);
            if (frag_pkt_head != IP_NULL)
                ipcom_pkt_free(frag_pkt_head);
            return IP_NULL;
        }

#ifdef IPCOM_USE_MIB2
        if(other_hdr_size == 0)
        {
#ifdef IPCOM_USE_INET
            Ipnet_netif *netif = ipnet_if_indextonetif(pkt->vr_index, pkt->ifindex);

            IPCOM_MIB2(ipFragCreates++);
            if (netif != IP_NULL)
            {
                IPCOM_MIB2_SYSWI_U32_ADD(v4, ipSystemStatsOutFragCreates, 1);
                IPCOM_MIB2_PERIF_U32_ADD(v4, ipIfStatsOutFragCreates, 1, netif);
            }
#endif
        }
        else
        {
#ifdef IPCOM_USE_INET6
            Ipnet_netif *netif = ipnet_if_indextonetif(pkt->vr_index, pkt->ifindex);

            IPCOM_MIB2_NETIF(ipv6IfStatsOutFragCreates++);
            if (netif != IP_NULL)
            {
                IPCOM_MIB2_SYSWI_U32_ADD(v6, ipSystemStatsOutFragCreates, 1);
                IPCOM_MIB2_PERIF_U32_ADD(v6, ipIfStatsOutFragCreates, 1, netif);
            }
#endif
        }
#endif /* IPCOM_USE_MIB2 */

        if (frag_pkt_head == IP_NULL)
            frag_pkt_head = frag_pkt;
        else
            frag_pkt_tail->next_fragment = frag_pkt;
        frag_pkt_tail = frag_pkt;

        IPCOM_PKT_TRACE(frag_pkt, IPCOM_PKT_ID_FRAG);
        frag_pkt->fd       = pkt->fd;
        frag_pkt->ifindex  = pkt->ifindex;
        frag_pkt->flags    = pkt->flags;
        frag_pkt->vr_index = pkt->vr_index;
        /* Copy IP packet payload */
        frag_pkt->start      = (frag_pkt->maxlen - size_of_this_fragment) & ~0x3;
        frag_pkt->end        = frag_pkt->start + size_of_this_fragment;
        ip_assert(frag_pkt->start < frag_pkt->maxlen);
        ipcom_memcpy(&frag_pkt->data[frag_pkt->start],
                     &pkt->data[pkt->start + frag_offset],
                     (Ip_size_t)size_of_this_fragment);

        frag_pkt->offset     = (Ip_u16) (frag_offset + pkt->offset);
        frag_offset += size_of_this_fragment;
        IP_BIT_SET(frag_pkt->flags, IPCOM_PKT_FLAG_MORE_FRAG);
    }

    if (IP_BIT_ISFALSE(pkt->flags, IPCOM_PKT_FLAG_FRAGMENT)
        || (more_fragments && IP_FALSE == more_fragments(pkt)))
        /* This is either an IP datagram sourced from this node or the
           last fragment of an IP datagram that has been forwarded by
           this node. */
        IP_BIT_CLR(frag_pkt_tail->flags, IPCOM_PKT_FLAG_MORE_FRAG);

    ip_assert(pkt->start + frag_offset == pkt->end);
    ip_assert(frag_pkt_head != IP_NULL);
    if (IP_NULL != (ssb = ipcom_pkt_get_info(pkt, IPNET_PKT_INFO_SOCK_SND_BUF)))
    {
        /* Transfer the socket send buffer allocation from the
           original packet to the last fragment to make sure that the
           send flow control is still driven by TX done events. */
        ipcom_pkt_set_info(frag_pkt_tail,
                           IPNET_PKT_INFO_SOCK_SND_BUF,
                           sizeof(Ipnet_pkt_info_sock_snd_buf),
                           ssb);
        frag_pkt_tail->dtor = pkt->dtor;
        pkt->dtor = IP_NULL;
    }

    /* Do not free original yet, since it might hold the
       callers signal buffer. Put it in the fragment tails
       next original instead and it will be freed when the
       last segment has been transmitted */
    if (frag_pkt_tail != IP_NULL)
        frag_pkt_tail->next_original = pkt;
    else
        ipcom_pkt_free(pkt);

    return frag_pkt_head;
}


/*
 *===========================================================================
 *                    ipnet_reassembly
 *===========================================================================
 * Description: Reassembles fragmented IP packets.
 *
 *                          ...                 ...
 *                           |                   |
 *                         pkt  #A             pkt  #B
 *                         frag #b             frag #y
 *                           ^                   ^
 *                           |                   |
 *                           |                   |
 *                      next_fragment       next_fragment
 *                           |                   |
 *                           |                   |
 *                    ---> pkt  #A -- next --> pkt  #B -- next --> ...
 *                    |    frag #a             frag #x
 *                    |
 *              reassembly_list
 *
 *              pkt #A chain will time out before pkt #B if not all fragments
 *              are received.
 *              frag #a and frag #x has lower offset than frag #b and frag #y.
 *              NOTE: this function must be called with the reassembly_list_lock
 *                    taken.
 * Parameters:  reassembly_list - Pointer to the head of the list of
 *                  reassembled packets.
 *              pkt - A newly received fragment.
 *              is_part_of_same_pkt - function that returns IP_TRUE if two
 *                  fragments is part of the same packet.
 *              get_offset - function that returns the offset of a within
 *                  the reassembled packet.
 *              more_fragments - function that returns if a fragment is the
 *                  last fragment of a packet.
 *              tmo_cb - handler for reassembly timeout of a packet.
 *              frag_hdr_len - size of the fragmentation header.
 * Returns:     Pointer to the reassembled packet or IP_NULL if more fragments
 *              is needed.
 */
IP_GLOBAL Ipcom_pkt *
ipnet_reassembly(Ipcom_pkt **reassembly_list,
                 Ipcom_pkt *pkt,
                 Ipnet_is_part_of_same_pkt_func is_part_of_same_pkt,
                 Ipnet_get_offset_func get_offset,
                 Ipnet_more_fragments_func more_fragments,
                 Ipnet_timeout_handler tmo_cb,
                 int frag_hdr_len)
{
    Ipcom_pkt **pkt_it;     /* The packet this fragment belongs to */
    Ipcom_pkt **frag_it;    /* The fragment within the selected packet this fragment
                               should inserted in-font of */
    Ipcom_pkt  *frag_head = IP_NULL;
    Ipcom_pkt  *tmo_pkt;
    Ipcom_pkt  *new_tmo_pkt;
    Ipcom_pkt  *drop_pkt_list;
    int         offset;
    int         previous_end;

    tmo_pkt = *reassembly_list;

    IPNET_PKT_SET_TMO(pkt, IP_NULL);
    offset = get_offset(pkt);
    for (pkt_it = reassembly_list; *pkt_it != IP_NULL; pkt_it = &(*pkt_it)->next)
    {
        if (is_part_of_same_pkt(pkt, *pkt_it))
            break;
    }

    if (*pkt_it == IP_NULL)
    {
        /* First fragment received of this packet, add it last to the list */
        *pkt_it = pkt;
        pkt->next = IP_NULL;
#if defined(IPNET_DEBUG) || defined(IPNET_STATISTICS)
#ifdef IPCOM_USE_INET
        if (is_part_of_same_pkt == ipnet_ip4_is_part_of_same_pkt)
        {
            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                              1, 2, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_RECV,
                              ipnet_reassembly, IPCOM_WV_NETD_INFO_RECEIVE,
                              IPCOM_WV_IPNET_FRAG_MODULE, IPCOM_WV_NETD_IP_REASSEMBLE);
            IPNET_STATS(pkt_num_ip4_reassembly_list++);
        }
#endif
#ifdef IPCOM_USE_INET6
        if (is_part_of_same_pkt == ipnet_ip6_is_part_of_same_pkt)
        {
            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP6_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                              1, 3, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_RECV,
                              ipnet_reassembly, IPCOM_WV_NETD_INFO_RECEIVE,
                              IPCOM_WV_IPNET_FRAG_MODULE, IPCOM_WV_NETD_IP_REASSEMBLE);
            IPNET_STATS(pkt_num_ip6_reassembly_list++);
        }
#endif
#endif
        ip_assert(pkt->next_fragment == IP_NULL);
        IPNET_PKT_SET_TIMEOUT_ABS(pkt, IPNET_SECONDS_ABS + ipnet_conf_reassembly_timeout);
    }
    else
    {
        previous_end = 0;
        for (frag_it = pkt_it; (*frag_it) != IP_NULL; frag_it = &(*frag_it)->next_fragment)
        {
            if (offset <= get_offset(*frag_it))
            {
                /* The new fragment should be in front of this fragment */
                break;
            }
            previous_end = (*frag_it)->end - (*frag_it)->start
                + get_offset(*frag_it) - frag_hdr_len;
        }

        if (previous_end > offset
            || (*frag_it
                && (offset == get_offset(*frag_it)
                    || offset + pkt->end - pkt->start - frag_hdr_len > get_offset(*frag_it))))
        {
            /* Overlapping segment or this fragment has already been received */
            ipcom_pkt_free(pkt);
            return IP_NULL;
        }

        /* The timeout is calculated from the point when the first fragment
           in this packet was received */
        IPNET_PKT_SET_TIMEOUT_ABS(pkt, IPNET_PKT_GET_TIMEOUT_ABS(*pkt_it));

        /* Insert the fragment into the fragment list */
        pkt->next_fragment = *frag_it;
        *frag_it = pkt;
        if (pkt->next_fragment != IP_NULL && pkt->next_fragment->next != IP_NULL)
        {
            /*
             * 'pkt->next_fragment' was the first fragment received
             * for this datagram, but the new fragment should be
             * in-front of that one and will become the new head of
             * that IP datagram.
             */
            pkt->next = pkt->next_fragment->next;
            pkt->next_fragment->next = IP_NULL;
        }
    }

    /*
     * Check if all fragments has been received
     */
    for (frag_it = pkt_it, offset = 0; (*frag_it) != IP_NULL; frag_it = &(*frag_it)->next_fragment)
    {
        ip_assert((offset & 0x7) == 0);
        if (offset != get_offset(*frag_it))
            break;

        if (!more_fragments(*frag_it))
        {
            /* All fragments was received */
            frag_head = *pkt_it;

            /* Remove the fragment list from reassembly list */
            *pkt_it = frag_head->next;
            frag_head->next = IP_NULL;
            break;
        }

        offset += (*frag_it)->end - (*frag_it)->start - frag_hdr_len;
    }

    if (frag_head != IP_NULL
        || (ipnet_frag_list_len(*reassembly_list) <= ipnet_conf_max_reassembly_list_len
            && ipnet_frag_dgram_list_len(*pkt_it) <= ipnet_conf_max_dgram_frag_list_len))
        /*
         * Number of individual IP-datagrams and fragments of a
         * specific IP datagram are within limits.
         */
        drop_pkt_list = IP_NULL;
    else
    {
        /*
         * Too many partially reassembled fragments or one of the IP
         * datagrams consists of too many fragments (DoS attack?).
         * Drop the oldest partly reassembled IP
         * datagram. drop_pkt_list can never be equal to frag_head
         * since the frag_head list is either IP_NULL (more fragments
         * needed) or already removed from the list.
         */
        drop_pkt_list = *reassembly_list;
        *reassembly_list = drop_pkt_list->next;
        drop_pkt_list->next = IP_NULL;
    }

    /*
     * The TMO packet will change if
     *  - the oldest fragment list is returned
     *  - if the new fragment is a new head in the oldest fragment list
     */
    new_tmo_pkt = *reassembly_list;
    if (new_tmo_pkt != tmo_pkt)
    {
        if (tmo_pkt)
            ipnet_timeout_cancel(IPNET_PKT_GET_TMO(tmo_pkt));

        if (new_tmo_pkt)
            (void) ipnet_timeout_schedule(IPNET_PKT_GET_TIMEOUT(new_tmo_pkt) * 1000,
                                          tmo_cb,
                                          new_tmo_pkt,
                                          IPNET_PKT_GET_TMO_PTR(new_tmo_pkt));
    }

    if (drop_pkt_list)
        ipcom_pkt_free(drop_pkt_list);

    /*
     * All fragments has not been received if frag_head == IP_NULL
     */
    return frag_head;
}


/*
 *===========================================================================
 *                    ipnet_create_reassembled_packet
 *===========================================================================
 * Description: Reassembles a list of fragments to the orginal packet and
 *              creates a new packet of it.
 * Parameters:  fragment_list - The first fragment in the list.
 *              ip_hdr_size - The size of the IP header.
 *              frag_hdr_size - The size of the header containing the fragmentation id.
 *              update_ip_header - A function that can finish the IP header
 *              of the reassembled packet.
 *              reassemble_err_func - Handler for reassembly error.
 * Returns:     The reassembled packet or IP_NULL if a packet big enough
 *              to hold the whole packet.
 */
IP_GLOBAL Ipcom_pkt *
ipnet_create_reassembled_packet(Ipcom_pkt *fragment_list,
                                int ip_hdr_size,
                                int frag_hdr_size,
                                Ipnet_update_ip_header_func update_ip_header,
                                Ipnet_reassemble_err_func report_reassemble_err)
{
    Ipcom_pkt *pkt = IP_NULL;
    Ipcom_pkt *pkt_it;
    int        total_size = 0;
    int        mtu;

    for (pkt_it = fragment_list; pkt_it != IP_NULL; pkt_it = pkt_it->next_fragment)
    {
        pkt_it->start += frag_hdr_size;
        total_size += pkt_it->end - pkt_it->start;
        ip_assert(pkt_it->ipstart <= pkt_it->start);
        ip_assert(pkt_it->start <= pkt_it->end);
    }

    if (total_size > 65535)
    {
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                          1, 4, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                          ipnet_create_reassembled_packet, IPCOM_WV_NETD_BADLEN,
                          IPCOM_WV_IPNET_FRAG_MODULE, IPCOM_WV_NETD_IP_REASSEMBLE);
        IPNET_STATS(pkt_reasm_too_big++);
        if (report_reassemble_err)
            report_reassemble_err(fragment_list);
        goto done;
    }

    total_size += ip_hdr_size;
    mtu = total_size;

    pkt = ipcom_pkt_malloc(mtu,
                           IP_FLAG_FC_STACKCONTEXT);
    if (pkt == IP_NULL || pkt->maxlen < total_size)
    {
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_CRITICAL,
                          1, 5, IPCOM_WV_NETDEVENT_CRITICAL, IPCOM_WV_NETD_RECV,
                          ipnet_create_reassembled_packet, IPCOM_WV_NETD_NOBUFS,
                          IPCOM_WV_IPNET_FRAG_MODULE, IPCOM_WV_NETD_IP_REASSEMBLE);
        IPNET_STATS(pkt_reasm_nomem++);
        if (pkt != IP_NULL)
        {
            ipcom_pkt_free(pkt);
            pkt = IP_NULL;
        }
        goto done;
    }

    pkt->start = fragment_list->ipstart;
    if (pkt->start + total_size > pkt->maxlen)
        pkt->start  = (pkt->maxlen - total_size) & ~0x3;
    pkt->ipstart    = pkt->start;
    pkt->end        = pkt->start + ip_hdr_size;
    pkt->ifindex    = fragment_list->ifindex;
    pkt->vr_index   = fragment_list->vr_index;
    pkt->flags      = fragment_list->flags;
    pkt->flags &= ~(IPCOM_PKT_FLAG_FRAGMENT
                    | IPCOM_PKT_FLAG_MORE_FRAG
                    | IPCOM_PKT_FLAG_HW_CHECKSUM
                    | IPCOM_PKT_FLAG_TL_CHECKSUM);

    /* Copy the IP header */
    ipcom_memcpy(&pkt->data[pkt->start],
                 &fragment_list->data[fragment_list->ipstart],
                 (Ip_size_t)ip_hdr_size);

    /* Copy the rest of the packet */
    for (pkt_it = fragment_list; pkt_it != IP_NULL; pkt_it = pkt_it->next_fragment)
    {
        ipcom_memcpy(&pkt->data[pkt->end],
                     &pkt_it->data[pkt_it->start],
                     (Ip_size_t)(pkt_it->end - pkt_it->start));
        pkt->end += pkt_it->end - pkt_it->start;
        ip_assert(pkt->end <= pkt->maxlen);
    }

    update_ip_header(pkt, fragment_list);

done:
    ipcom_pkt_free(fragment_list);
    return pkt;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
